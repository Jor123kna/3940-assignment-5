#include "Socket.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

int main() {
    string host, path, filePath, caption, date, answer;
    int port;

    cout << "Host (localhost if both programs are in Linux): ";
    getline(cin, host);
    cout << "Port (8888 for C++, 8082 for Java, 8081 for Tomcat): ";
    cin >> port;
    cin.ignore(1000, '\n');
    cout << "Path (/ for C++ or Java, /trivia1/upload for Tomcat): ";
    getline(cin, path);
    cout << "File path: ";
    getline(cin, filePath);
    cout << "Caption: ";
    getline(cin, caption);
    cout << "Date (YYYY-MM-DD): ";
    getline(cin, date);
    cout << "Use the class Tomcat project? (y/n): ";
    getline(cin, answer);

    try {
        if (!cin || host.empty() || path.empty() || path[0] != '/' ||
            caption.empty() || date.empty())
            throw runtime_error("Please fill in all fields.");

        ifstream file(filePath, ios::binary);
        if (!file) throw runtime_error("Cannot open the file.");
        ostringstream contents;
        contents << file.rdbuf();
        string filename = filePath.substr(filePath.find_last_of("/\\") + 1);

        // Tomcat uses different field names from the Java and C++ servers.
        string captionField = "caption", dateField = "date", fileField = "fileName";
        if (answer == "y" || answer == "Y") {
            captionField = "Question";
            dateField = "UploadDate";
            fileField = "FileName";
        }

        string boundary = "----MyBoundary123";
        string body = "--" + boundary + "\r\n";
        body += "Content-Disposition: form-data; name=\"" + captionField + "\"\r\n\r\n";
        body += caption + "\r\n--" + boundary + "\r\n";
        body += "Content-Disposition: form-data; name=\"" + dateField + "\"\r\n\r\n";
        body += date + "\r\n--" + boundary + "\r\n";
        body += "Content-Disposition: form-data; name=\"" + fileField +
                "\"; filename=\"" + filename + "\"\r\n";
        body += "Content-Type: application/octet-stream\r\n\r\n";
        body += contents.str();
        body += "\r\n--" + boundary + "--\r\n";

        ostringstream headers;
        headers << "POST " << path << " HTTP/1.1\r\n";
        headers << "Host: " << host << ":" << port << "\r\n";
        headers << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n";
        headers << "Content-Length: " << body.size() << "\r\n";
        headers << "Connection: close\r\n\r\n";

        Socket socket(host, port);
        socket.sendAll(headers.str() + body);
        string response = socket.readToEnd();
        cout << response << endl;
        // A response such as HTTP/1.1 400 means the upload failed.
        istringstream status(response);
        string version;
        int code = 0;
        status >> version >> code;
        if (code != 200) return 1;
    } catch (const exception& error) {
        cerr << error.what() << endl;
        return 1;
    }
    return 0;
}
