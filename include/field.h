#ifndef FIELD_H
#define FIELD_H

#include <string>
#include <chrono>

class Field {
public:
    Field();
    void plant(const std::string& type, double growth_time = 10.0);
    void update(double dt);
    bool harvest(std::string& harvested_type);
    bool is_empty() const { return type_ == "empty"; }
    bool is_ready() const { return ready_; }
    const std::string& get_type() const { return type_; }

private:
    std::string type_;
    double growth_time_;
    std::chrono::steady_clock::time_point start_time_;
    bool ready_;
};

#endif