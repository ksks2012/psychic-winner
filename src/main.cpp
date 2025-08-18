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

// Game state
struct Field {
    std::string type; // "fire_grass", "wood_grass", "empty"
    double growth_time; // Growth time (seconds)
    std::chrono::steady_clock::time_point start_time;
    bool ready; // Is it harvestable
};

struct Game {
    std::vector<Field> fields; // 4x4 field
    std::map<std::string, int> inventory; // Inventory: fire_grass, wood_grass, pill
    int proficiency; // Proficiency
    bool refining; // Is refining
    double refine_time; // Refining timer
};

// Initialize SDL2 and game
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

// Load game state
void load_game(Game& game) {
    std::ifstream file("save.json");
    if (file.is_open()) {
        json j;
        file >> j;
        game.fields.clear();
        for (const auto& f : j["fields"]) {
            Field field;
            field.type = f["type"];
            field.growth_time = f["growth_time"];
            field.ready = f["ready"];
            field.start_time = std::chrono::steady_clock::now();
            game.fields.push_back(field);
        }
        game.inventory = j["inventory"].get<std::map<std::string, int>>();
        game.proficiency = j["proficiency"];
    } else {
        game.fields.resize(16, {"empty", 10.0, std::chrono::steady_clock::now(), false});
        game.inventory = {{"fire_grass", 0}, {"wood_grass", 0}, {"pill", 0}};
        game.proficiency = 0;
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
    std::ofstream file("save.json");
    file << j.dump(4);
}

// Update game logic
void update(Game& game, double dt) {
    // Update field growth
    for (auto& field : game.fields) {
        if (field.type != "empty" && !field.ready) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - field.start_time).count();
            if (elapsed >= field.growth_time) {
                field.ready = true;
            }
        }
    }
    // Simulate pest event
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    if (dis(gen) < 0.01) { // 1% chance
        for (auto& field : game.fields) {
            if (field.type != "empty" && !field.ready) {
                field.type = "empty"; // Pest clears field
                std::cout << "Pest! Field lost!" << std::endl;
            }
        }
    }
}

// Render fields and UI
void render(SDL_Renderer* renderer, TTF_Font* font, const Game& game) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render 4x4 fields
    for (int i = 0; i < 16; ++i) {
        int x = (i % 4) * 100 + 50;
        int y = (i / 4) * 100 + 50;
        SDL_Rect rect = {x, y, 80, 80};
        SDL_SetRenderDrawColor(renderer, game.fields[i].ready ? 0 : 255, game.fields[i].ready ? 255 : 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Render inventory
    SDL_Color white = {255, 255, 255, 255};
    for (const auto& [item, count] : game.inventory) {
        std::string text = item + ": " + std::to_string(count);
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst = {500, 50 + static_cast<int>(&item - &game.inventory.begin()->first) * 30, 200, 30};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

// Main game loop
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

        // Handle input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x / 100, y = event.button.y / 100;
                if (x >= 0 && x < 4 && y >= 0 && y < 4) {
                    int idx = y * 4 + x;
                    if (game.fields[idx].type == "empty") {
                        game.fields[idx].type = "fire_grass"; // Simple planting
                        game.fields[idx].start_time = std::chrono::steady_clock::now();
                        game.fields[idx].ready = false;
                    } else if (game.fields[idx].ready) {
                        game.inventory[game.fields[idx].type]++;
                        game.fields[idx].type = "empty";
                    }
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