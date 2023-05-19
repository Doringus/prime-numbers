#include <iostream>
#include <csignal>

#include "consumer.hpp"
#include "producer.hpp"

std::shared_ptr<Consumer> consumer;
std::shared_ptr<Producer> producer;

void sigintHandler(int signal) {
    producer->forceShutdown();
    consumer->forceShutdown();
}

int main() {
    std::signal(SIGINT, sigintHandler);
    consumer = std::make_shared<Consumer>(4);
    producer = std::make_shared<Producer>(consumer, std::vector<size_t>{1024, 1024, 100});

    producer->start();
    auto result =  consumer->waitForResult();
    std::cout << "Prime numbers count: " << result << "\n";

    return 0;
}