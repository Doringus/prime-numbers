#include <iostream>
#include <csignal>
#include <string_view>
#include <charconv>

#include "consumer.hpp"
#include "producer.hpp"

std::shared_ptr<Consumer> consumer;
std::shared_ptr<Producer> producer;

void sigintHandler(int signal) {
    producer->forceShutdown();
    consumer->forceShutdown();
}

int main(int argc, char** argv) {
    size_t consumerWorkers = 1;
    std::vector<size_t> eventsToProduce {100};

    std::vector<std::string_view> args(argv + 1, argv + argc);

    for(size_t i = 0; i < args.size(); ++i) {
        if(args[i].find("-c") != std::string_view::npos) {
            std::from_chars(args[i + 1].data(), args[i + 1].data() + args[i + 1].length(), consumerWorkers);
        } else if(args[i].find("-e") != std::string_view::npos) {
            if(args[i + 1].size() > 0) {
                eventsToProduce.clear();
            }
            size_t commaPos = 0;
            while( (commaPos = args[i + 1].find(',')) != std::string_view::npos) {
                size_t value;
                std::from_chars(args[i + 1].data(), args[i + 1].data() + commaPos, value);
                eventsToProduce.push_back(value);
                args[i + 1].remove_prefix(commaPos + 1);
            }
            size_t value;
            std::from_chars(args[i + 1].data(), args[i + 1].data() + args[i + 1].size(), value);
            eventsToProduce.push_back(value);
        }
    }

    std::signal(SIGINT, sigintHandler);
    consumer = std::make_shared<Consumer>(consumerWorkers);
    producer = std::make_shared<Producer>(consumer, eventsToProduce);

    producer->start();
    auto result =  consumer->waitForResult();
    std::cout << "Prime numbers count: " << result << "\n";

    return 0;
}