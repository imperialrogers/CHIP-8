#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <cstdint>

class Platform
{
public:
    // Constructor
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

    // Destructor
    ~Platform();

    void Update(void const* buffer, int pitch);

    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};    
};

#endif