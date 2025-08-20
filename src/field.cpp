#include "field.h"
#include <iostream>

Field::Field() : type_(Constants::EMPTY), growth_time_(Constants::DEFAULT_GROWTH_TIME), ready_(false) {}

void Field::plant(const std::string& type, double growth_time) {
    type_ = type;
    growth_time_ = growth_time;
    start_time_ = std::chrono::steady_clock::now();
    ready_ = false;
    std::cout << "Planted " << type_ << std::endl;
}

void Field::update(double dt) {
    if (type_ != Constants::EMPTY && !ready_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time_).count() / Constants::MILLISECONDS_TO_SECONDS;
        if (elapsed >= growth_time_) {
            ready_ = true;
            std::cout << "Field is ready for harvest: " << type_ << std::endl;
        }
    }
}

bool Field::harvest(std::string& harvested_type) {
    if (ready_ && type_ != Constants::EMPTY) {
        harvested_type = type_;
        type_ = Constants::EMPTY;
        ready_ = false;
        std::cout << "Harvested " << harvested_type << std::endl;
        return true;
    }
    return false;
}