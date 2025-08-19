#include "game.h"
#include "renderer.h"
#include "savemanager.h"
#include <SDL2/SDL.h>
#include <chrono>

int main(int argc, char* argv[]) {
    Renderer renderer;
    if (!renderer.init()) return 1;

    Game game;
    SaveManager::load(game);
    bool running = true;
    SDL_Event event;
    auto last_time = std::chrono::steady_clock::now();

    while (running) {
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count() / 1000.0;
        last_time = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;
                if (x >= 50 && x < 450 && y >= 50 && y < 450) {
                    int idx = ((y - 50) / 100) * 4 + ((x - 50) / 100);
                    if (idx >= 0 && idx < 16) {
                        std::string harvested_type;
                        if (!game.plant(idx, "fire_grass")) {
                            game.harvest(idx, harvested_type);
                        }
                    }
                } else if (x >= 500 && x <= 600 && y >= 200 && y <= 240) {
                    game.start_refining();
                } else if (x >= 500 && x <= 600 && y >= 250 && y <= 290) {
                    std::string flame = game.get_flame_type();
                    if (flame == "low") game.set_flame_type("mid");
                    else if (flame == "mid") game.set_flame_type("high");
                    else game.set_flame_type("low");
                }
            }
        }

        game.update(dt);
        renderer.render(game);
        SaveManager::save(game);
        SDL_Delay(16);
    }

    return 0;
}