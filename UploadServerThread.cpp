#include "UploadServerThread.hpp"
#include "UploadServlet.hpp"
#include <cctype>
#include <iostream>
#include <stdexcept>
using namespace std;

// Find the data belonging to one multipart form field.
string getPart(const string& body, const string& boundary, const string& name) {
    size_t start = body.find("; name=\"" + name + "\"");
    if (start == string::npos) throw runtime_error("Missing form field.");
    start = body.find("\r\n\r\n", start);
    if (start == string::npos) throw runtime_error("Invalid form data.");
    start += 4;
    size_t end = body.find("\r\n" + boundary, start);
    if (end == string::npos) throw runtime_error("Missing boundary.");
    return body.substr(start, end - start);
}

UploadServerThread::UploadServerThread(Socket* socket) : socket(socket) {}

UploadServerThread::~UploadServerThread() {
    delete socket;
}

void UploadServerThread::run() {
    try {
        string method, path, version;
        istringstream firstLine(socket->readLine());
        firstLine >> method >> path >> version;

        string line, headers;
        int length = 0;
        while (!(line = socket->readLine()).empty()) {
            headers += line + "\r\n";
            string lower = line;
            for (size_t i = 0; i < lower.size(); i++)
                lower[i] = tolower((unsigned char) lower[i]);
            if (lower.find("content-length:") == 0)
                length = stoi(line.substr(15));
            if (lower == "expect: 100-continue")
                socket->sendAll("HTTP/1.1 100 Continue\r\n\r\n");
        }
        if (length < 0 || length > 32 * 1024 * 1024)
            throw runtime_error("Invalid upload size.");

        stringstream input(headers + "\r\n" + socket->readBytes(length));
        HttpServletRequest request(input);
        ostringstream output;
        HttpServletResponse response(output);
        UploadServlet servlet;

        if (method == "GET" && path == "/") {
            servlet.doGet(request, response);
        } else if (method == "POST") {
            parseRequest(input, request);
            servlet.doPost(request, response);
        } else {
            output << "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        }
        socket->sendAll(output.str());
    } catch (const exception& error) {
        cerr << error.what() << endl;
        try {
            socket->sendAll("HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\nInvalid upload.");
        } catch (...) {
            // The client may already have disconnected.
        }
    }
}

stringstream& UploadServerThread::parseRequest(stringstream& input, HttpServletRequest& request) {
    string line, boundary;
    while (getline(input, line) && line != "\r" && !line.empty()) {
        size_t position = line.find("boundary=");
        if (position != string::npos) {
            boundary = line.substr(position + 9);
            if (!boundary.empty() && boundary.back() == '\r') boundary.pop_back();
            if (boundary.size() >= 2 && boundary.front() == '"' && boundary.back() == '"')
                boundary = boundary.substr(1, boundary.size() - 2);
        }
    }
    if (boundary.empty()) throw runtime_error("Missing boundary.");
    boundary = "--" + boundary;

    ostringstream contents;
    contents << input.rdbuf();
    string body = contents.str();
    request.setCaption(getPart(body, boundary, "caption"));
    request.setDate(getPart(body, boundary, "date"));

    size_t start = body.find("; name=\"fileName\"");
    if (start == string::npos) throw runtime_error("Missing file.");
    start = body.find("filename=\"", start);
    if (start == string::npos) throw runtime_error("Missing filename.");
    start += 10;
    size_t end = body.find('"', start);
    if (end == string::npos) throw runtime_error("Invalid filename.");
    request.setFilename(body.substr(start, end - start));

    // The request stream now contains only the file's binary bytes.
    input.str(getPart(body, boundary, "fileName"));
    input.clear();
    return input;
}
