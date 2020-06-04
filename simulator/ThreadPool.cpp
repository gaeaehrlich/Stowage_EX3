#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) : _numTasks(0) {
    auto thread_loop = [&](size_t id) {
        while (!_stopped) {
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
    _threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; i++) {
        _threads.push_back(std::thread(thread_loop, i));
    }
}

ThreadPool::~ThreadPool() {
    _stopped = true;
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