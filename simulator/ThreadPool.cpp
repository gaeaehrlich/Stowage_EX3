#include <iostream>
#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) : _numTasks(0) {
    auto thread_loop = [&](size_t id) {
        while (_running) {
            _mutex.lock();
            if (!_tasks.empty()) {
                std::cout << "I am " << id << ". num tasks: " << _numTasks << std::endl;
                std::cout << "Received a task" << std::endl;
                auto work = _tasks.front();
                _tasks.pop();
                std::cout << "popped" << std::endl;
                _mutex.unlock();
                work();
                _numTasks --;
                std::cout << "Done operating task" << std::endl;
            } else {
                _mutex.unlock();
                std::this_thread::yield();
            }
        }
    };
    _threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; i++) {
        _threads.push_back(std::thread(thread_loop, i));
    }
}

ThreadPool::~ThreadPool() {
    _running = false;
    for (std::thread& t : _threads) {
        t.join();
    }
}

void ThreadPool::addTask(Task task) {
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