#pragma once

#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

using Task = std::function<void()>;

class TaskQueue final {
public:

    void push(Task&& task);
    bool tryPush(Task&& task);

    void pop(Task& task);
    bool tryPop(Task& task);
private:
    void takeLocked(Task& task);

private:
    std::deque<Task> m_Tasks;
    mutable std::mutex m_Mutex; // Guards m_Tasks
    std::condition_variable m_NotEmpty;
};