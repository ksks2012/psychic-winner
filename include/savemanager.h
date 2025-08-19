#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "game.h"
#include <nlohmann/json.hpp>

class SaveManager {
public:
    static void load(Game& game);
    static void save(const Game& game);
};

#endif