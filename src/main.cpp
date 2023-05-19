#include <iostream>
#include "consumer.hpp"
#include "producer.hpp"

int main() {
    auto consumer = std::make_shared<Consumer>(4);
    auto producer = std::make_shared<Producer>(consumer, std::vector<size_t>{1024, 1024, 100});

    producer->start();
    consumer->stop();
    std::cout << "Prime numbers count: " << consumer->getResult() << "\n";

    return 0;
}