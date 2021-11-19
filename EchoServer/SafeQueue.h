#include <queue>
#include <mutex>
#include <condition_variable>

#pragma once

template <class T>
class SafeQueue
{
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;
public:
    SafeQueue()
    {
        queue = std::queue<T>();
    }

    ~SafeQueue(){}

    // Add an element to the queue.
    void enqueue(T item)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(item);
        condition.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty())
        {
            // release lock as long as the wait and reaquire it afterwards.
            condition.wait(lock);
        }
        T val = queue.front();
        queue.pop();
        return val;
    }

    bool isEmpty()
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }
};