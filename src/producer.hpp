#pragma once

#include <thread>
#include <vector>
#include <memory>

#include "consumer.hpp"

class Producer final {
public:
    Producer(std::shared_ptr<Consumer> taskSystem, const std::vector<size_t>& eventsToGenerate);

    /**
     * Blocks calling thread until all events are sent
     */
    void start();

    /**
     * Shutdown producer even if not all events are sent
     */
    void forceShutdown();
private:
    void workerRoutine(size_t eventsToGenerate, const std::atomic_bool& isRunning, std::shared_ptr<Consumer> taskSystem) const;

private:
    std::shared_ptr<Consumer> m_TaskSystem;
    std::vector<size_t> m_EventsToGenerate;
    std::vector<std::thread> m_Workers;
    std::atomic_bool m_IsRunning;
};