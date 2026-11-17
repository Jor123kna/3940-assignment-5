#include "Thread.h"
#include <cstring>
#include <exception>
#include <iostream>
#include <pthread.h>
#include <stdexcept>

void* Thread::startMethod(void* argument) {
    Thread* worker = static_cast<Thread*>(argument);
    try { worker->run(); }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; }
    delete worker;
    return nullptr;
}

void Thread::start() {
    pthread_attr_t attributes;
    int error = pthread_attr_init(&attributes);
    if (error != 0) throw std::runtime_error(std::strerror(error));
    error = pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    pthread_t id;
    if (error == 0) error = pthread_create(&id, &attributes, startMethod, this);
    pthread_attr_destroy(&attributes);
    if (error != 0) throw std::runtime_error(std::strerror(error));
}
