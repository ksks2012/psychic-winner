#include "savemanager.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void SaveManager::load(Game& game) {
    std::ifstream file("save.json");
    if (file.is_open()) {
        try {
            json j;
            file >> j;
            std::cout << "Loading save.json..." << std::endl;
            game = Game(); // Reset to default state
            for (const auto& f : j["fields"]) {
                int idx = j["fields"].size();
                if (idx < 16) {
                    std::string type = f.contains("type") && !f["type"].is_null() ? f["type"].get<std::string>() : "empty";
                    double growth_time = f.contains("growth_time") && !f["growth_time"].is_null() ? f["growth_time"].get<double>() : 10.0;
                    game.plant(idx, type);
                    if (f.contains("ready") && !f["ready"].is_null() && f["ready"].get<bool>()) {
                        game.get_fields()[idx].update(growth_time); // Simulate growth to ready
                    }
                }
            }
            game = Game();
            game.get_inventory() = j.contains("inventory") && !j["inventory"].is_null() ?
                                   j["inventory"].get<std::map<std::string, int>>() :
                                   std::map<std::string, int>{{"fire_grass", 0}, {"wood_grass", 0}, {"pill", 0}};
            game.set_proficiency(j.contains("proficiency") && !j["proficiency"].is_null() ? j["proficiency"].get<int>() : 0);
            game.set_flame_type(j.contains("flame_type") && !j["flame_type"].is_null() ? j["flame_type"].get<std::string>() : "low");
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
        j["fields"].push_back({{"type", f.get_type()}, {"growth_time", 10.0}, {"ready", f.is_ready()}});
    }
    j["inventory"] = game.get_inventory();
    j["proficiency"] = game.get_proficiency();
    j["flame_type"] = game.get_flame_type();
    std::ofstream file("save.json");
    file << j.dump(4);
}