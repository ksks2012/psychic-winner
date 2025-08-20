#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include "constants.h"

enum class Screen { FIELD, INVENTORY, REFINING };

class Renderer {
public:
    Renderer();
    ~Renderer();
    bool init();
    void render(const Game& game, Screen screen);

private:
    void render_field_screen(const Game& game);
    void render_inventory_screen(const Game& game);
    void render_refining_screen(const Game& game);
    void render_navigation_buttons(Screen current_screen);
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
};

#endif