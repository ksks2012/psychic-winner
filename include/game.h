#ifndef GAME_H
#define GAME_H

#include "field.h"
#include <vector>
#include <map>
#include <string>

class Game {
public:
    Game();
    void update(double dt);
    bool plant(int idx, const std::string& type);
    bool harvest(int idx, std::string& harvested_type);
    bool start_refining();
    void set_flame_type(const std::string& flame);
    const std::vector<Field>& get_fields() const { return fields_; }
    std::vector<Field>& get_fields() { return fields_; }
    const std::map<std::string, int>& get_inventory() const { return inventory_; }
    std::map<std::string, int>& get_inventory() { return inventory_; }
    const std::string& get_flame_type() const { return flame_type_; }
    int get_proficiency() const { return proficiency_; }
    void set_proficiency(int proficiency) { proficiency_ = proficiency; }

private:
    bool refine();
    std::vector<Field> fields_;
    std::map<std::string, int> inventory_;
    int proficiency_;
    bool refining_;
    double refine_time_;
    std::string flame_type_;
    double pest_timer_;
};

#endif