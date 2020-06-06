#include <iostream>
#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) : _numTasks(0), _numThreads(numThreads) {}

void ThreadPool::startThreads() {
    auto thread_loop = [&](size_t id) {
        while (_running) {
            _mutex.lock();
            if (!_tasks.empty()) {
                auto work = _tasks.front();
                _tasks.pop();
                _mutex.unlock();
                work();
                _numTasks --;
            } else {
                _mutex.unlock();
                std::this_thread::yield();
            }
        }
    };
    _threads.reserve(_numThreads);
    for (size_t i = 0; i < _numThreads; i++) {
        _threads.push_back(std::thread(thread_loop, i));
    }
}

ThreadPool::~ThreadPool() {
    _running = false;
    for (std::thread& t : _threads) {
        t.join();
    }
}

void ThreadPool::addTask(const Task& task) {
    _mutex.lock();
    _tasks.push(task);
    _numTasks++;
    _mutex.unlock();
}

void ThreadPool::joinThreads() {
    while (_numTasks.load() > 0) {
        std::this_thread::yield();
    }
}