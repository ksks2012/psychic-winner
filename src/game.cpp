#include "game.h"
#include <random>
#include <iostream>

Game::Game() : fields_(16), inventory_{{"fire_grass", 0}, {"wood_grass", 0}, {"pill", 0}},
               proficiency_(0), refining_(false), refine_time_(0.0), flame_type_("low"), pest_timer_(0.0) {}

void Game::update(double dt) {
    for (auto& field : fields_) {
        field.update(dt);
    }
    if (refining_) {
        refine_time_ -= dt;
        if (refine_time_ <= 0) {
            bool success = refine();
            std::cout << "Refining " << (success ? "succeeded!" : "failed!") << std::endl;
            refining_ = false;
        }
    }
    pest_timer_ += dt;
    if (pest_timer_ >= 5.0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) < 0.01) {
            for (auto& field : fields_) {
                if (!field.is_empty() && !field.is_ready()) {
                    field.plant("empty");
                    std::cout << "Pest attack! Spirit grass lost!" << std::endl;
                }
            }
        }
        pest_timer_ = 0.0;
    }
}

bool Game::plant(int idx, const std::string& type) {
    if (idx >= 0 && idx < fields_.size() && fields_[idx].is_empty()) {
        fields_[idx].plant(type);
        return true;
    }
    return false;
}

bool Game::harvest(int idx, std::string& harvested_type) {
    if (idx >= 0 && idx < fields_.size() && fields_[idx].is_ready()) {
        if (fields_[idx].harvest(harvested_type)) {
            inventory_[harvested_type]++;
            return true;
        }
    }
    return false;
}

bool Game::start_refining() {
    if (!refining_ && inventory_["fire_grass"] >= 2) {
        refining_ = true;
        refine_time_ = 5.0;
        std::cout << "Started refining with " << flame_type_ << " flame" << std::endl;
        return true;
    }
    std::cout << "Not enough fire_grass or already refining!" << std::endl;
    return false;
}

void Game::set_flame_type(const std::string& flame) {
    flame_type_ = flame;
    std::cout << "Flame set to " << flame_type_ << std::endl;
}

bool Game::refine() {
    if (inventory_["fire_grass"] < 2) return false;
    inventory_["fire_grass"] -= 2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double success_rate = 0.5 + proficiency_ * 0.01;
    if (flame_type_ == "mid") success_rate += 0.1;
    else if (flame_type_ == "high") success_rate += 0.2;
    if (dis(gen) < success_rate) {
        inventory_["pill"]++;
        proficiency_ += 5;
        return true;
    }
    return false;
}