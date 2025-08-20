#include "game.h"
#include "renderer.h"
#include "savemanager.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <chrono>

int main(int argc, char* argv[]) {
    Renderer renderer;
    if (!renderer.init()) return 1;

    Game game;
    SaveManager::load(game);
    bool running = true;
    Screen current_screen = Screen::FIELD;
    SDL_Event event;
    auto last_time = std::chrono::steady_clock::now();

    while (running) {
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count() / Constants::MILLISECONDS_TO_SECONDS;
        last_time = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;
                // Navigation buttons
                if (x >= Constants::WINDOW_WIDTH - 150 && x < Constants::WINDOW_WIDTH - 110 && y >= 10 && y < 50) {
                    current_screen = Screen::FIELD;
                } else if (x >= Constants::WINDOW_WIDTH - 100 && x < Constants::WINDOW_WIDTH - 60 && y >= 10 && y < 50) {
                    current_screen = Screen::INVENTORY;
                } else if (x >= Constants::WINDOW_WIDTH - 50 && x < Constants::WINDOW_WIDTH - 10 && y >= 10 && y < 50) {
                    current_screen = Screen::REFINING;
                }
                // Screen-specific interactions
                if (current_screen == Screen::FIELD && x >= Constants::FIELD_START_X && x < Constants::FIELD_START_X + Constants::FIELD_GRID_WIDTH &&
                    y >= Constants::FIELD_START_Y && y < Constants::FIELD_START_Y + Constants::FIELD_GRID_HEIGHT) {
                    int idx = ((y - Constants::FIELD_START_Y) / Constants::FIELD_SPACING) * Constants::GRID_SIZE +
                             ((x - Constants::FIELD_START_X) / Constants::FIELD_SPACING);
                    if (idx >= 0 && idx < Constants::FIELD_COUNT) {
                        std::string harvested_type;
                        if (!game.plant(idx, Constants::FIRE_GRASS)) {
                            game.harvest(idx, harvested_type);
                        }
                    }
                } else if (current_screen == Screen::REFINING) {
                    if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_X + Constants::BUTTON_WIDTH &&
                        y >= Constants::REFINE_BUTTON_Y && y <= Constants::REFINE_BUTTON_Y + Constants::BUTTON_HEIGHT) {
                        game.start_refining();
                    } else if (x >= Constants::BUTTON_X && x <= Constants::BUTTON_X + Constants::BUTTON_WIDTH &&
                               y >= Constants::FLAME_BUTTON_Y && y <= Constants::FLAME_BUTTON_Y + Constants::BUTTON_HEIGHT) {
                        std::string flame = game.get_flame_type();
                        if (flame == Constants::LOW_FLAME) game.set_flame_type(Constants::MID_FLAME);
                        else if (flame == Constants::MID_FLAME) game.set_flame_type(Constants::HIGH_FLAME);
                        else game.set_flame_type(Constants::LOW_FLAME);
                    }
                }
            }
        }

        game.update(dt);
        renderer.render(game, current_screen);
        SaveManager::save(game);
        SDL_Delay(Constants::FRAME_DELAY);
    }

    return 0;
}