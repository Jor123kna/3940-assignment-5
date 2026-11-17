#include "ServerSocket.h"
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

ServerSocket::ServerSocket(int port) : sock(-1) {
    if (port < 1 || port > 65535) throw std::invalid_argument("Port must be 1 to 65535.");
    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) throw std::runtime_error("Cannot create server socket.");
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(static_cast<unsigned short>(port));
    if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0 ||
        listen(sock, 20) < 0) {
        const std::string message = std::strerror(errno);
        ::close(sock);
        throw std::runtime_error("Cannot listen: " + message);
    }
}

Socket* ServerSocket::Accept() {
    int client;
    do { client = ::accept(sock, nullptr, nullptr); } while (client < 0 && errno == EINTR);
    if (client < 0) throw std::runtime_error("Cannot accept connection.");
    return new Socket(client);
}

ServerSocket::~ServerSocket() { ::close(sock); }
