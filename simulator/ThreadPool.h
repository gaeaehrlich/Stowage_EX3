#ifndef STOWAGE_THREADPOOL_H
#define STOWAGE_THREADPOOL_H

#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include "../algorithm/AbstractAlgorithm.h"

using std::string;
using std::unique_ptr;
using std::pair;
using std::vector;


typedef std::function<void(void)> Task;

class ThreadPool {
private:
    int _numThreads;
    std::atomic<bool> _running = true;
    std::queue<Task> _tasks;
    std::atomic<int> _numTasks;
    std::mutex _mutex;
    vector<std::thread> _threads;

public:
    ThreadPool(int numThreads);
    void startThreads();
    virtual ~ThreadPool();
    void addTask(const Task& task);
    void joinThreads();
};

#endif //STOWAGE_THREADPOOL_H
