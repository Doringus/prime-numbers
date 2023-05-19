#pragma once

#include <future>
#include <chrono>

#include "taskqueue.hpp"

struct event_t {
    int value;
    std::time_t time;
};

/**
 * Task stealing system
 */
class Consumer final {
public:
    explicit Consumer(size_t workersCount);
    ~Consumer();

    template<typename T>
    void submit(T&& task) {
        auto i = m_WorkerIndex++;
        for (size_t n = 0; n < m_WorkersCount; ++n) {
            if (m_Queues[(i + n) % m_WorkersCount].tryPush(std::forward<T>(task))) {
                return;
            }
        }
        m_Queues[i % m_WorkersCount].push(std::forward<T>(task));
    }

    /**
     * Blocks calling thread untill all events are processed
     */
    size_t waitForResult();

    /**
     * Unblocks thread in waitForResult ASAP
     */
    void forceShutdown();

private:
    size_t workerRoutine(size_t workerIndex, std::promise<size_t> result);
    bool isPrime(size_t value) const noexcept;

private:
    bool m_Stopped {false};
    size_t m_WorkersCount;
    std::atomic<size_t> m_WorkerIndex {0};
    std::vector<std::future<size_t>> m_Result;
    std::vector<TaskQueue<event_t>> m_Queues;
    std::vector<std::thread> m_Workers;
};