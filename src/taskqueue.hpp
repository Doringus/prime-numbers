#pragma once

#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class TaskQueue final {
public:

    template<typename Task>
    void push(Task&& task) {
        {
            std::unique_lock lock {m_Mutex};
            m_Tasks.emplace_back(std::forward<Task>(task));
        }
        m_NotEmpty.notify_one();
    }

    template<typename Task>
    bool tryPush(Task&& task) {
        {
            std::unique_lock lock {m_Mutex, std::defer_lock};
            if(!lock.try_lock()) {
                return false;
            }
            m_Tasks.emplace_back(std::forward<Task>(task));
        }
        m_NotEmpty.notify_one();
        return true;
    }

    bool pop(T& task) {
        std::unique_lock lock {m_Mutex };

        while(m_Tasks.empty() && !m_IsDone) {
            m_NotEmpty.wait(lock);
        }
        if (m_Tasks.empty()) {
            return false;
        }
        takeLocked(task);
        return true;
    }

    bool tryPop(T& task) {
        {
            std::unique_lock lock {m_Mutex, std::defer_lock};
            if(!lock.try_lock() || m_Tasks.empty()) {
                return false;
            }
            takeLocked(task);
        }
        return true;
    }

    void done() {
        {
            std::unique_lock lock{ m_Mutex };
            m_IsDone = true;
        }
        m_NotEmpty.notify_all();
    }
private:
    void takeLocked(T& task) {
        task = std::move(m_Tasks.front());
        m_Tasks.pop_front();
    }

private:
    bool m_IsDone {false};
    std::deque<T> m_Tasks;
    mutable std::mutex m_Mutex; // Guards m_Tasks
    std::condition_variable m_NotEmpty;
};