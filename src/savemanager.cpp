#include "savemanager.h"
#include "constants.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void SaveManager::load(Game& game) {
    std::ifstream file(Constants::SAVE_FILE);
    if (file.is_open()) {
        try {
            json j;
            file >> j;
            std::cout << "Loading save.json..." << std::endl;
            game = Game(); // Reset to default state
            for (const auto& f : j["fields"]) {
                int idx = j["fields"].size();
                if (idx < Constants::FIELD_COUNT) {
                    std::string type = f.contains("type") && !f["type"].is_null() ? f["type"].get<std::string>() : Constants::EMPTY;
                    double growth_time = f.contains("growth_time") && !f["growth_time"].is_null() ? f["growth_time"].get<double>() : Constants::DEFAULT_GROWTH_TIME;
                    game.plant(idx, type);
                    if (f.contains("ready") && !f["ready"].is_null() && f["ready"].get<bool>()) {
                        game.get_fields()[idx].update(growth_time); // Simulate growth to ready
                    }
                }
            }
            game = Game();
            game.get_inventory() = j.contains("inventory") && !j["inventory"].is_null() ?
                                   j["inventory"].get<std::map<std::string, int>>() :
                                   Constants::DEFAULT_INVENTORY;
            game.set_proficiency(j.contains("proficiency") && !j["proficiency"].is_null() ? j["proficiency"].get<int>() : 0);
            game.set_flame_type(j.contains("flame_type") && !j["flame_type"].is_null() ? j["flame_type"].get<std::string>() : Constants::LOW_FLAME);
            std::cout << "Loaded game: flame_type=" << game.get_flame_type() << ", fields=" << game.get_fields().size() << std::endl;
        } catch (const json::exception& e) {
            std::cerr << "JSON Load Error: " << e.what() << std::endl;
            std::cout << "Creating new game state due to JSON error" << std::endl;
            game = Game();
        }
    } else {
        std::cout << "No save.json found, creating new game state" << std::endl;
        game = Game();
    }
}

void SaveManager::save(const Game& game) {
    json j;
    j["fields"] = json::array();
    for (const auto& f : game.get_fields()) {
        j["fields"].push_back({{"type", f.get_type()}, {"growth_time", Constants::DEFAULT_GROWTH_TIME}, {"ready", f.is_ready()}});
    }
    j["inventory"] = game.get_inventory();
    j["proficiency"] = game.get_proficiency();
    j["flame_type"] = game.get_flame_type();
    std::ofstream file(Constants::SAVE_FILE);
    file << j.dump(Constants::JSON_INDENT);
}