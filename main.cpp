#include "Chip8.hpp"
#include "Platform.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
using namespace std;

int main(int argc, char** argv)
{
    // Check for correct command to run the executable with sufficient arguments
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = stoi(argv[1]);
    // int videoScale = 10;

    int cycleDelay = stoi(argv[2]);
    // int cycleDelay = 1;

    char const* romFilename = argv[3];
    // char const* romFilename = "test_opcode.ch8";


    // Instantiate SDL2 based graphical platform
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale,
                      VIDEO_WIDTH, VIDEO_HEIGHT);

    // Instantiate Chip-8 Emulation Engine 
    Chip8 chip8;

    // Load the ROM
    chip8.LoadROM(romFilename);

    // Specify the bytes occupied by a single row of display (size of one pixel multiplied by Width)
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now(); // Starting time
    bool quit = false; // variable to check if the exit condition is true

    // Run the emulation cycles in loop until exit condition becomes true
    while(!quit)
    {
        // Register key input
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();

        // Variable to store elapsed time between last cycle and this one
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay)
        {
            // Update the last cycle time to current time
            lastCycleTime = currentTime;

            // Execute the emulation cycle
            chip8.Cycle();

            // Update the display
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}



// const int WIDTH = 800, HEIGHT = 600;

// int main( int argc, char *argv[] )
// {
//     SDL_Init( SDL_INIT_EVERYTHING );

//     SDL_Window *window = SDL_CreateWindow( "Hello SDL World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI );
    
//     // Check that the window was successfully created
//     if ( NULL == window )
//     {
//         // In the case that the window could not be made...
//         std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
//         return 1;
//     }
    
//     SDL_Event windowEvent;
    
//     while ( true )
//     {
//         if ( SDL_PollEvent( &windowEvent ) )
//         {
//             if ( SDL_QUIT == windowEvent.type )
//             {
//                 break;
//             }
//         }
//     }
    
//     SDL_DestroyWindow( window );
//     SDL_Quit( );
    
//     return EXIT_SUCCESS;
// }