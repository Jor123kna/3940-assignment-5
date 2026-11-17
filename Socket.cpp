#include "Socket.h"
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

Socket::Socket(int descriptor) : sock(descriptor) {
    if (sock < 0) throw runtime_error("Invalid socket.");
}

Socket::Socket(const string& host, int port) : sock(-1) {
    if (port < 1 || port > 65535) throw runtime_error("Invalid port.");
    hostent* address = gethostbyname(host.c_str());
    if (address == NULL) throw runtime_error("Host not found.");
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy(&server.sin_addr, address->h_addr, address->h_length);
    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) throw runtime_error("Cannot create socket.");
    if (connect(sock, (sockaddr*) &server, sizeof(server)) < 0) {
        closeSocket();
        throw runtime_error("Cannot connect to server.");
    }
}

string Socket::readLine() {
    string line;
    char character;
    while (line.size() < 8192) {
        int count = recv(sock, &character, 1, 0);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) throw runtime_error("Connection ended early.");
        if (character == '\n') return line;
        if (character != '\r') line += character;
    }
    throw runtime_error("HTTP line is too long.");
}

string Socket::readBytes(size_t length) {
    string data(length, '\0');
    size_t received = 0;
    while (received < length) {
        int count = recv(sock, &data[received], length - received, 0);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) throw runtime_error("Incomplete upload.");
        received += count;
    }
    return data;
}

string Socket::readToEnd() {
    string data;
    char buffer[4096];
    int count;
    while (true) {
        count = recv(sock, buffer, sizeof(buffer), 0);
        if (count < 0 && errno == EINTR) continue;
        if (count < 0) throw runtime_error("Cannot read response.");
        if (count == 0) return data;
        data.append(buffer, count);
    }
}

void Socket::sendAll(const string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        int count = send(sock, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) throw runtime_error("Cannot send data.");
        sent += count;
    }
}

void Socket::closeSocket() {
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}

Socket::~Socket() {
    closeSocket();
}
