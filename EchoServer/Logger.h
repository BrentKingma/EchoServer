#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#pragma once

class Logger
{
private:
    bool runThread;
    std::queue<std::string> queue;
    std::mutex mutex;
    std::condition_variable condition;
    std::thread logThread;
public:
    Logger()
    {
        queue = std::queue<std::string>();
        runThread = true;
        logThread = std::thread([this]() {
            while (runThread)
            {
                if (!isEmpty())
                {
                    std::cout << dequeue() << std::endl;
                }
            }
            });
    }

    ~Logger()
    {
        runThread = false;
        logThread.join();
    }

    // Add an element to the queue.
    void enqueue(std::string item)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(item);
        condition.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    std::string dequeue(void)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty())
        {
            std::string msg = "";
            return msg;
        }

        std::string val = queue.front();
        queue.pop();
        return val;
    }

    bool isEmpty()
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }
};
