#include <iostream>
#include "consumer.hpp"
#include "producer.hpp"

int main() {
    auto consumer = std::make_shared<Consumer>(3);
    auto producer = std::make_shared<Producer>(consumer, std::vector<size_t>{43, 55});

    producer->start();
    consumer->stop();
    std::cout << "Prime numbers count: " << consumer->getResult() << "\n";

    return 0;
}