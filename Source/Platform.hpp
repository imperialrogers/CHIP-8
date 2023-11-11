#pragma once
#include <cstdint>
#include "../CHIP-8/SDL/include/SDL2/SDL.h"

using namespace std;

class Platform{
    public:
        //
    private:
        SDL_Window* window{};
        SDL_Renderer* renderer{};
        SDL_Texture* texture{};
};