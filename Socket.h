#pragma once
#include <cstddef>
#include <string>

// Owns one Linux socket. Used by both the client and server.
class Socket {
public:
    explicit Socket(int descriptor);
    Socket(const std::string& host, int port);
    ~Socket();
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    std::string readLine();
    std::string readBytes(std::size_t length);
    std::string readToEnd();
    void sendAll(const std::string& data);
    void closeSocket();

private:
    int sock;
};
