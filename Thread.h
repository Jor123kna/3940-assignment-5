#pragma once

class Thread {
public:
    virtual ~Thread() = default;
    virtual void run() = 0;

    // Start only heap-allocated workers. The detached thread deletes its worker
    // after run() returns, so main can accept the next connection immediately.
    void start();

private:
    static void* startMethod(void* argument);
};
