#include "ServerSocket.h"
#include "UploadServerThread.hpp"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    try {
        int port = 8888;
        if (argc == 2) port = stoi(argv[1]);
        ServerSocket server(port);
        cout << "UploadServer running on port " << port << endl;
        while (true) {
            UploadServerThread* worker = new UploadServerThread(server.Accept());
            try {
                worker->start();
            } catch (...) {
                delete worker;
                throw;
            }
        }
    } catch (const exception& error) {
        cerr << error.what() << endl;
        return 1;
    }
}
