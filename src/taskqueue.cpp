#include "taskqueue.hpp"

void TaskQueue::pop(Task &task) {
    std::unique_lock lock {m_Mutex };
    m_NotEmpty.wait(lock, [this]() {
        return !m_Tasks.empty();
    });
}

void TaskQueue::takeLocked(Task &task) {
    task = std::move(m_Tasks.front());
    m_Tasks.pop_front();
}

bool TaskQueue::tryPop(Task &task) {
    return false;
}

bool TaskQueue::tryPush(Task &&task) {
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

void TaskQueue::push(Task &&task) {
    {
        std::unique_lock lock {m_Mutex};
        m_Tasks.emplace_back(std::forward<Task>(task));
    }
    m_NotEmpty.notify_one();
}
