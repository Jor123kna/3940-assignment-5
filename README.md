# Upload Assignment

This keeps the server classes requested by the assignment. Each class has a header
(declarations) and a .cpp file (implementation), which is why there are several files.
ConsoleApp.cpp is the entire client. No testing files or extra design patterns are needed.

In Ubuntu / WSL, open this folder and run:

```sh
make
./UploadServer
```

In another Ubuntu terminal in this folder:

```sh
./ConsoleApp
```

Answer the prompts. Use port 8888, path /, and answer n for Tomcat to upload to the C++ server.
Files are saved in the server's images folder.

For a browser, open http://localhost:8888/ while the server runs.
If Windows localhost does not work, run hostname -I in Ubuntu and use that address.

To use the Java server, run it separately and enter port 8082, path / and n in the C++ client.
For the installed Tomcat project, use port 8081, path /trivia1/upload and y.
Its existing Oracle database setup is still needed for database storage.

If Java or Tomcat runs on Windows, use the Windows address after "via" in
`ip route show default` as the C++ client's host.
For a Windows Java client uploading to C++, use the Ubuntu address from `hostname -I`.
Run `./UploadServer 8082` to match the supplied Java client's port.

The original HttpOverTCP .cc files are classroom examples; the Makefile does not use them.

For debugger practice, run `gdb ./UploadServer`, then:
```text
break UploadServlet::doPost
run
```
Upload from another terminal. Use next to step, continue to resume, and quit to exit.
