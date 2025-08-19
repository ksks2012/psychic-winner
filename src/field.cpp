#include "field.h"
#include <iostream>

Field::Field() : type_("empty"), growth_time_(10.0), ready_(false) {}

void Field::plant(const std::string& type, double growth_time) {
    type_ = type;
    growth_time_ = growth_time;
    start_time_ = std::chrono::steady_clock::now();
    ready_ = false;
    std::cout << "Planted " << type_ << std::endl;
}

void Field::update(double dt) {
    if (type_ != "empty" && !ready_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time_).count() / 1000.0;
        if (elapsed >= growth_time_) {
            ready_ = true;
            std::cout << "Field is ready for harvest: " << type_ << std::endl;
        }
    }
}

bool Field::harvest(std::string& harvested_type) {
    if (ready_ && type_ != "empty") {
        harvested_type = type_;
        type_ = "empty";
        ready_ = false;
        std::cout << "Harvested " << harvested_type << std::endl;
        return true;
    }
    return false;
}