#include "UploadServlet.hpp"
#include <dirent.h>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
using namespace std;

// Keep a form value inside a single filename.
string safeName(string value) {
    for (size_t i = 0; i < value.size(); i++)
        if (value[i] == '/' || value[i] == '\\' || value[i] == '\r' || value[i] == '\n')
            value[i] = '_';
    return value;
}

string htmlText(const string& value) {
    string result;
    for (size_t i = 0; i < value.size(); i++) {
        if (value[i] == '&') result += "&amp;";
        else if (value[i] == '<') result += "&lt;";
        else if (value[i] == '>') result += "&gt;";
        else result += value[i];
    }
    return result;
}

void UploadServlet::doGet(HttpServletRequest, HttpServletResponse response) {
    response.getOutputStream()
        << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
        << "<html><body><h2>Upload a File</h2>"
        << "<form action=\"/\" method=\"post\" enctype=\"multipart/form-data\">"
        << "Caption: <input name=\"caption\" required><br>"
        << "Date: <input type=\"date\" name=\"date\" required><br>"
        << "<input type=\"file\" name=\"fileName\" required><br>"
        << "<input type=\"submit\" value=\"Upload\"></form></body></html>";
}

void UploadServlet::doPost(HttpServletRequest request, HttpServletResponse response) {
    if (request.getCaption().empty() || request.getDate().empty() || request.getFilename().empty())
        throw runtime_error("Caption, date and file are required.");

    mkdir("images", 0755);
    string filename = request.getFilename();
    filename = filename.substr(filename.find_last_of("/\\") + 1);
    string savedName = safeName(request.getCaption()) + "_" +
                       safeName(request.getDate()) + "_" + safeName(filename);
    ofstream file("images/" + savedName, ios::binary);
    if (!file) {
        response.getOutputStream()
            << "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\nCannot save file.";
        return;
    }
    string bytes = request.getInputStream().str();
    file.write(bytes.data(), bytes.size());
    file.close();
    if (!file) throw runtime_error("Could not finish saving the file.");

    ostringstream& out = response.getOutputStream();
    out << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
    out << "<html><body><h2>Uploaded Files</h2><ul>";
    DIR* directory = opendir("images");
    if (directory != NULL) {
        dirent* entry;
        while ((entry = readdir(directory)) != NULL) {
            string name = entry->d_name;
            if (name != "." && name != "..")
                out << "<li>" << htmlText(name) << "</li>";
        }
        closedir(directory);
    }
    out << "</ul></body></html>";
}
