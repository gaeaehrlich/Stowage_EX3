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
    std::atomic<bool> _stopped = false;
    std::queue<Task> _tasks;
    std::atomic<int> _numTasks;
    std::mutex _mutex;
    vector<std::thread> _threads;
public:
    ThreadPool(int numThreads);
    virtual ~ThreadPool();
    void addTask(Task task);
};

#endif //STOWAGE_THREADPOOL_H
