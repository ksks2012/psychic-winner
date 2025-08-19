#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    bool init();
    void render(const Game& game);

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
};

#endif