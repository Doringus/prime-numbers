#include "producer.hpp"

#include <random>

Producer::Producer(std::shared_ptr<Consumer> taskSystem, const std::vector<size_t>& eventsToGenerate)
    : m_TaskSystem(std::move(taskSystem)),  m_EventsToGenerate(eventsToGenerate) { }

void Producer::start() {
    m_IsRunning = true;
    for(auto& count : m_EventsToGenerate) {
        m_Workers.emplace_back([this, count](){
            workerRoutine(count, m_IsRunning, m_TaskSystem);
        });
    }

    for(auto& w : m_Workers) {
        w.join();
    }
}


void Producer::workerRoutine(size_t eventsToGenerate, const std::atomic_bool& isRunning,
                             std::shared_ptr<Consumer> taskSystem) const {
    std::random_device rd;
    std::mt19937 generator {rd()};
    std::uniform_int_distribution dist {0, 1'000'000};

    while(eventsToGenerate != 0 && m_IsRunning) {
        int value = dist(generator);
        event_t event {value};
        m_TaskSystem->submit(event);
        eventsToGenerate--;
    }
}
