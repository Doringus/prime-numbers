#include "consumer.hpp"

#include <cmath>
#include <numeric>

Consumer::Consumer(size_t workersCount) : m_WorkersCount(workersCount), m_Queues(workersCount), m_Result(workersCount) {
    for(size_t i = 0; i < workersCount; ++i) {
        m_Workers.emplace_back([this, i](){
            std::promise<size_t> promise;
            m_Result[i] = promise.get_future();
            workerRoutine(i, std::move(promise));
        });
    }
}

Consumer::~Consumer() {
    if(!m_Stopped) {
        for(auto& w : m_Workers) {
            w.join();
        }
    }
}

void Consumer::workerRoutine(size_t workerIndex, std::promise<size_t> result) {
    size_t primeNumsCount = 0;
    while(true) {
        event_t task;
        bool taskTaken = false;
        for(size_t i = 0; i < m_Workers.size(); ++i) {
            if(m_Queues[(i + workerIndex) % m_Workers.size()].tryPop(task)) {
                taskTaken = true;
                break;
            }
        }

        if(!taskTaken && !m_Queues[workerIndex].pop(task)) {
            // we are done
            break;
        }

        if(isPrime(task.value)) {
            primeNumsCount++;
        }
    }

    result.set_value(primeNumsCount);
}

bool Consumer::isPrime(size_t value) const noexcept {
    if(value == 1) {
        return false;
    }
    for(size_t i = 2; i < std::sqrt(value); ++i) {
        if(value % i == 0) {
            return false;
        }
    }
    return true;
}

size_t Consumer::waitForResult() {
    for(auto& q : m_Queues) {
        q.done();
    }

    return std::accumulate(m_Result.begin(), m_Result.end(), 0,
                       [] (size_t value,auto& e) {
        return value + e.get();
    });
}

void Consumer::forceShutdown() {
    m_Stopped = true;
    for(auto& q : m_Queues) {
        q.forceShutdown();
    }

    for(auto& w : m_Workers) {
        w.join();
    }
}

