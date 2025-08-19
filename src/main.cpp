#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

struct Field {
    std::string type;
    double growth_time;
    std::chrono::steady_clock::time_point start_time;
    bool ready;
};

struct Game {
    std::vector<Field> fields; // 4x4 field
    std::map<std::string, int> inventory; // Inventory: fire_grass, wood_grass, pill
    int proficiency; // Proficiency
    bool refining; // Is refining
    double refine_time; // Refining timer
    std::string flame_type; // type of flame
};

// Initialize SDL2
bool init(SDL_Window*& window, SDL_Renderer*& renderer, TTF_Font*& font) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "IMG_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("Alchemist MVP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }
    font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        std::cerr << "Font Error: " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}

// Load game with error handling
void load_game(Game& game) {
    std::ifstream file("save.json");
    if (file.is_open()) {
        try {
            json j;
            file >> j;
            std::cout << "Loading save.json..." << std::endl;
            game.fields.clear();
            for (const auto& f : j["fields"]) {
                Field field;
                field.type = f.contains("type") && !f["type"].is_null() ? f["type"].get<std::string>() : "empty";
                field.growth_time = f.contains("growth_time") && !f["growth_time"].is_null() ? f["growth_time"].get<double>() : 10.0;
                field.ready = f.contains("ready") && !f["ready"].is_null() ? f["ready"].get<bool>() : false;
                field.start_time = std::chrono::steady_clock::now();
                game.fields.push_back(field);
            }
            game.inventory = j.contains("inventory") && !j["inventory"].is_null() ? 
                            j["inventory"].get<std::map<std::string, int>>() : 
                            std::map<std::string, int>{{"fire_grass", 0}, {"wood_grass", 0}, {"pill", 0}};
            game.proficiency = j.contains("proficiency") && !j["proficiency"].is_null() ? j["proficiency"].get<int>() : 0;
            game.flame_type = j.contains("flame_type") && !j["flame_type"].is_null() ? j["flame_type"].get<std::string>() : "low";
            std::cout << "Loaded game: flame_type=" << game.flame_type << ", fields=" << game.fields.size() << std::endl;
        } catch (const json::exception& e) {
            std::cerr << "JSON Load Error: " << e.what() << std::endl;
            std::cout << "Creating new game state due to JSON error" << std::endl;
            game.fields.resize(16, {"empty", 10.0, std::chrono::steady_clock::now(), false});
            game.inventory = {{"fire_grass", 0}, {"wood_grass", 0}, {"pill", 0}};
            game.proficiency = 0;
            game.flame_type = "low";
        }
    } else {
        std::cout << "No save.json found, creating new game state" << std::endl;
        game.fields.resize(16, {"empty", 10.0, std::chrono::steady_clock::now(), false});
        game.inventory = {{"fire_grass", 0}, {"wood_grass", 0}, {"pill", 0}};
        game.proficiency = 0;
        game.flame_type = "low";
    }
}

// Save game
void save_game(const Game& game) {
    json j;
    j["fields"] = json::array();
    for (const auto& f : game.fields) {
        j["fields"].push_back({{"type", f.type}, {"growth_time", f.growth_time}, {"ready", f.ready}});
    }
    j["inventory"] = game.inventory;
    j["proficiency"] = game.proficiency;
    j["flame_type"] = game.flame_type;
    std::ofstream file("save.json");
    file << j.dump(4);
}

// Refining logic
bool refine(Game& game) {
    if (game.inventory["fire_grass"] < 2) {
        std::cout << "Not enough fire_grass for refining!" << std::endl;
        return false;
    }
    game.inventory["fire_grass"] -= 2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double success_rate = 0.5 + game.proficiency * 0.01;
    if (game.flame_type == "mid") success_rate += 0.1;
    else if (game.flame_type == "high") success_rate += 0.2;
    if (dis(gen) < success_rate) {
        game.inventory["pill"]++;
        game.proficiency += 5;
        return true;
    }
    return false;
}

// Update logic
void update(Game& game, double dt) {
    for (size_t i = 0; i < game.fields.size(); ++i) {
        auto& field = game.fields[i];
        if (field.type != "empty" && !field.ready) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - field.start_time).count() / 1000.0;
            if (elapsed >= field.growth_time) {
                field.ready = true;
                std::cout << "Field " << i << " is ready for harvest!" << std::endl;
            }
        }
    }
    if (game.refining) {
        game.refine_time -= dt;
        if (game.refine_time <= 0) {
            bool success = refine(game);
            std::cout << "Refining " << (success ? "succeeded!" : "failed!") << std::endl;
            game.refining = false;
        }
    }
    static double pest_timer = 0.0;
    pest_timer += dt;
    if (pest_timer >= 5.0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) < 0.01) {
            for (auto& field : game.fields) {
                if (field.type != "empty" && !field.ready) {
                    field.type = "empty";
                    std::cout << "Pest attack! Spirit grass lost!" << std::endl;
                }
            }
        }
        pest_timer = 0.0;
    }
}

// Render
void render(SDL_Renderer* renderer, TTF_Font* font, const Game& game) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < game.fields.size(); ++i) {
        int x = (i % 4) * 100 + 50;
        int y = (i / 4) * 100 + 50;
        SDL_Rect rect = {x, y, 80, 80};
        if (game.fields[i].type == "empty") {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        } else if (game.fields[i].ready) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_Color white = {255, 255, 255, 255};
    int y_offset = 50;
    for (const auto& [item, count] : game.inventory) {
        std::string text = item + ": " + std::to_string(count);
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst = {500, y_offset, 200, 30};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y_offset += 30;
    }
    std::string flame_text = "Flame: " + game.flame_type;
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, flame_text.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {500, y_offset, 200, 30};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect button = {500, 200, 100, 40};
    SDL_RenderFillRect(renderer, &button);
    surface = TTF_RenderUTF8_Solid(font, "Refine", white);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    dst = {510, 210, 80, 30};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    button = {500, 250, 100, 40};
    SDL_RenderFillRect(renderer, &button);
    surface = TTF_RenderUTF8_Solid(font, "Toggle Flame", white);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    dst = {510, 260, 80, 30};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
}

// Main loop
int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    if (!init(window, renderer, font)) return 1;

    Game game;
    load_game(game);
    bool running = true;
    SDL_Event event;
    auto last_time = std::chrono::steady_clock::now();

    while (running) {
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count() / 1000.0;
        last_time = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;
                if (x >= 50 && x < 450 && y >= 50 && y < 450) {
                    int idx = ((y - 50) / 100) * 4 + ((x - 50) / 100);
                    if (idx >= 0 && idx < 16) {
                        if (game.fields[idx].type == "empty") {
                            game.fields[idx].type = "fire_grass";
                            game.fields[idx].start_time = std::chrono::steady_clock::now();
                            game.fields[idx].ready = false;
                            std::cout << "Planted fire_grass at field " << idx << std::endl;
                        } else if (game.fields[idx].ready) {
                            game.inventory[game.fields[idx].type]++;
                            game.fields[idx].type = "empty";
                            game.fields[idx].ready = false;
                            std::cout << "Harvested fire_grass from field " << idx << std::endl;
                        }
                    }
                } else if (x >= 500 && x <= 600 && y >= 200 && y <= 240 && !game.refining) {
                    game.refining = true;
                    game.refine_time = 5.0;
                    std::cout << "Started refining with " << game.flame_type << " flame" << std::endl;
                } else if (x >= 500 && x <= 600 && y >= 250 && y <= 290) {
                    if (game.flame_type == "low") game.flame_type = "mid";
                    else if (game.flame_type == "mid") game.flame_type = "high";
                    else game.flame_type = "low";
                    std::cout << "Flame set to " << game.flame_type << std::endl;
                }
            }
        }

        update(game, dt);
        render(renderer, font, game);
        save_game(game);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}