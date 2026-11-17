CXX = g++
CXXFLAGS = -Wall -g -pthread

SERVER = UploadServer.cpp UploadServerThread.cpp UploadServlet.cpp HttpServlet.cpp \
         HttpServletRequest.cpp HttpServletResponse.cpp ServerSocket.cpp Socket.cpp Thread.cpp
HEADERS = $(wildcard *.h *.hpp)

all: UploadServer ConsoleApp

UploadServer: $(SERVER) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SERVER) -o UploadServer

ConsoleApp: ConsoleApp.cpp Socket.cpp Socket.h
	$(CXX) $(CXXFLAGS) ConsoleApp.cpp Socket.cpp -o ConsoleApp

clean:
	rm -f UploadServer ConsoleApp *.o *.d

.PHONY: all clean
