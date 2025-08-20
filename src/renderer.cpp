#include "renderer.h"
#include "constants.h"
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
    window_ = SDL_CreateWindow("Alchemist MVP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 0);
    if (!window_) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }
    font_ = TTF_OpenFont(Constants::FONT_PATH.c_str(), Constants::FONT_SIZE);
    if (!font_) {
        std::cerr << "Font Error: " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}

void Renderer::render(const Game& game) {
    SDL_SetRenderDrawColor(renderer_, Constants::WHITE.r, Constants::WHITE.g, Constants::WHITE.b, Constants::WHITE.a);
    SDL_RenderClear(renderer_);

    for (size_t i = 0; i < game.get_fields().size(); ++i) {
        int x = (i % Constants::GRID_SIZE) * Constants::FIELD_SPACING + Constants::FIELD_START_X;
        int y = (i / Constants::GRID_SIZE) * Constants::FIELD_SPACING + Constants::FIELD_START_Y;
        SDL_Rect rect = {x, y, Constants::FIELD_SIZE, Constants::FIELD_SIZE};
        if (game.get_fields()[i].is_empty()) {
            SDL_SetRenderDrawColor(renderer_, Constants::GRAY.r, Constants::GRAY.g, Constants::GRAY.b, Constants::GRAY.a);
        } else if (game.get_fields()[i].is_ready()) {
            SDL_SetRenderDrawColor(renderer_, Constants::RED.r, Constants::RED.g, Constants::RED.b, Constants::RED.a);
        } else {
            SDL_SetRenderDrawColor(renderer_, Constants::GREEN.r, Constants::GREEN.g, Constants::GREEN.b, Constants::GREEN.a);
        }
        SDL_RenderFillRect(renderer_, &rect);
    }

    SDL_Color white = {Constants::WHITE.r, Constants::WHITE.g, Constants::WHITE.b, Constants::WHITE.a};
    int y_offset = Constants::INVENTORY_START_Y;
    for (const auto& [item, count] : game.get_inventory()) {
        std::string text = item + ": " + std::to_string(count);
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font_, text.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_Rect dst = {Constants::INVENTORY_X, y_offset, 200, Constants::TEXT_HEIGHT};
        SDL_RenderCopy(renderer_, texture, nullptr, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y_offset += Constants::TEXT_HEIGHT;
    }
    std::string flame_text = "Flame: " + game.get_flame_type();
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font_, flame_text.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_Rect dst = {Constants::INVENTORY_X, y_offset, 200, Constants::TEXT_HEIGHT};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_SetRenderDrawColor(renderer_, Constants::BLUE.r, Constants::BLUE.g, Constants::BLUE.b, Constants::BLUE.a);
    SDL_Rect button = {Constants::BUTTON_X, Constants::REFINE_BUTTON_Y, Constants::BUTTON_WIDTH, Constants::BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer_, &button);
    surface = TTF_RenderUTF8_Solid(font_, "Refine", white);
    texture = SDL_CreateTextureFromSurface(renderer_, surface);
    dst = {Constants::BUTTON_X + 10, Constants::REFINE_BUTTON_Y + 10, Constants::BUTTON_WIDTH - 20, Constants::TEXT_HEIGHT};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_SetRenderDrawColor(renderer_, Constants::CYAN.r, Constants::CYAN.g, Constants::CYAN.b, Constants::CYAN.a);
    button = {Constants::BUTTON_X, Constants::FLAME_BUTTON_Y, Constants::BUTTON_WIDTH, Constants::BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer_, &button);
    surface = TTF_RenderUTF8_Solid(font_, "Toggle Flame", white);
    texture = SDL_CreateTextureFromSurface(renderer_, surface);
    dst = {Constants::BUTTON_X + 10, Constants::FLAME_BUTTON_Y + 10, Constants::BUTTON_WIDTH - 20, Constants::TEXT_HEIGHT};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer_);
}