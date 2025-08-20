#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <map>
#include <SDL2/SDL.h>

namespace Constants {
    // Game logic constants
    constexpr int FIELD_COUNT = 16;
    constexpr int GRID_SIZE = 4;
    constexpr double DEFAULT_GROWTH_TIME = 10.0;
    constexpr double PEST_CHECK_INTERVAL = 5.0;
    constexpr double PEST_ATTACK_PROBABILITY = 0.01;
    constexpr double REFINE_TIME = 5.0;
    constexpr int FIRE_GRASS_REQUIRED = 2;
    constexpr double BASE_SUCCESS_RATE = 0.5;
    constexpr double PROFICIENCY_BONUS = 0.01;
    constexpr double MID_FLAME_BONUS = 0.1;
    constexpr double HIGH_FLAME_BONUS = 0.2;
    constexpr int PROFICIENCY_GAIN = 5;
    
    // UI constants
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    constexpr int FIELD_START_X = 50;
    constexpr int FIELD_START_Y = 50;
    constexpr int FIELD_SIZE = 80;
    constexpr int FIELD_SPACING = 100;
    constexpr int FIELD_GRID_WIDTH = 400;
    constexpr int FIELD_GRID_HEIGHT = 400;
    
    // Button positions
    constexpr int BUTTON_X = 500;
    constexpr int REFINE_BUTTON_Y = 200;
    constexpr int FLAME_BUTTON_Y = 250;
    constexpr int BUTTON_WIDTH = 100;
    constexpr int BUTTON_HEIGHT = 40;
    
    // Text display
    constexpr int INVENTORY_X = 500;
    constexpr int INVENTORY_START_Y = 50;
    constexpr int TEXT_HEIGHT = 30;
    constexpr int FONT_SIZE = 24;
    
    // Colors (RGBA)
    struct Color {
        unsigned char r, g, b, a;
    };
    
    constexpr Color WHITE = {255, 255, 255, 255};
    constexpr Color GRAY = {200, 200, 200, 255};
    constexpr Color RED = {255, 0, 0, 255};
    constexpr Color GREEN = {0, 255, 0, 255};
    constexpr Color BLUE = {0, 0, 255, 255};
    constexpr Color CYAN = {0, 255, 255, 255};
    
    // Game timing
    constexpr int FRAME_DELAY = 16; // ~60 FPS
    constexpr double MILLISECONDS_TO_SECONDS = 1000.0;
    
    // File paths
    const std::string SAVE_FILE = "save.json";
    const std::string FONT_PATH = "assets/font.ttf";
    
    // Default flame types
    const std::string LOW_FLAME = "low";
    const std::string MID_FLAME = "mid";
    const std::string HIGH_FLAME = "high";
    
    // Item names
    const std::string FIRE_GRASS = "fire_grass";
    const std::string WOOD_GRASS = "wood_grass";
    const std::string PILL = "pill";
    const std::string EMPTY = "empty";
    
    // Default inventory
    const std::map<std::string, int> DEFAULT_INVENTORY = {
        {FIRE_GRASS, 0},
        {WOOD_GRASS, 0},
        {PILL, 0}
    };
    
    // JSON formatting
    constexpr int JSON_INDENT = 4;
        
    inline SDL_Color toSDLColor(const Color& color) {
        return SDL_Color{color.r, color.g, color.b, color.a};
    }
}

#endif
