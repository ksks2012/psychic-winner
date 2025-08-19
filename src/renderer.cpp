#include "renderer.h"
#include <SDL2/SDL_image.h>
#include <iostream>

Renderer::Renderer() : window_(nullptr), renderer_(nullptr), font_(nullptr) {}

Renderer::~Renderer() {
    if (font_) TTF_CloseFont(font_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool Renderer::init() {
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
    window_ = SDL_CreateWindow("Alchemist MVP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window_) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }
    font_ = TTF_OpenFont("assets/font.ttf", 24);
    if (!font_) {
        std::cerr << "Font Error: " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}

void Renderer::render(const Game& game) {
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderClear(renderer_);

    for (size_t i = 0; i < game.get_fields().size(); ++i) {
        int x = (i % 4) * 100 + 50;
        int y = (i / 4) * 100 + 50;
        SDL_Rect rect = {x, y, 80, 80};
        if (game.get_fields()[i].is_empty()) {
            SDL_SetRenderDrawColor(renderer_, 200, 200, 200, 255);
        } else if (game.get_fields()[i].is_ready()) {
            SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 255);
        }
        SDL_RenderFillRect(renderer_, &rect);
    }

    SDL_Color white = {255, 255, 255, 255};
    int y_offset = 50;
    for (const auto& [item, count] : game.get_inventory()) {
        std::string text = item + ": " + std::to_string(count);
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font_, text.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_Rect dst = {500, y_offset, 200, 30};
        SDL_RenderCopy(renderer_, texture, nullptr, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y_offset += 30;
    }
    std::string flame_text = "Flame: " + game.get_flame_type();
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font_, flame_text.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_Rect dst = {500, y_offset, 200, 30};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_SetRenderDrawColor(renderer_, 0, 0, 255, 255);
    SDL_Rect button = {500, 200, 100, 40};
    SDL_RenderFillRect(renderer_, &button);
    surface = TTF_RenderUTF8_Solid(font_, "Refine", white);
    texture = SDL_CreateTextureFromSurface(renderer_, surface);
    dst = {510, 210, 80, 30};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_SetRenderDrawColor(renderer_, 0, 255, 255, 255);
    button = {500, 250, 100, 40};
    SDL_RenderFillRect(renderer_, &button);
    surface = TTF_RenderUTF8_Solid(font_, "Toggle Flame", white);
    texture = SDL_CreateTextureFromSurface(renderer_, surface);
    dst = {510, 260, 80, 30};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer_);
}