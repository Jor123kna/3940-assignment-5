#pragma once
#include "Socket.h"
class ServerSocket
{
public:
	ServerSocket(int port);
	Socket* Accept();
	~ServerSocket();
	ServerSocket(const ServerSocket&) = delete;
	ServerSocket& operator=(const ServerSocket&) = delete;
private:
	int sock;
};

