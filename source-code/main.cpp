#include "Chip8.hpp"
#include "Platform.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
using namespace std;

int main(int inputSize, char** input)
{
    // Check for correct command to run the executable with sufficient arguments
    if (inputSize != 4)
    {
        cout<<"ENTER THE ROM IN PROPER FORMAT"<<endl;
        cout << "FORMAT OF USE: " << input[0] << " <Scale> <Delay> <ROM>\n";
        exit(EXIT_FAILURE);
    }

    int videoScaling = stoi(input[1]);
    int cycleDelay = stoi(input[2]);
    char const* ROM = input[3];

    // Instantiate SDL2 based graphical screen
    Platform screen("CHIP-8 Emulator", VIDEO_WIDTH * videoScaling, VIDEO_HEIGHT * videoScaling, VIDEO_WIDTH, VIDEO_HEIGHT);

    // Instantiate Chip-8 Emulation Engine 
    Chip8 chip8;

    // Load the ROM
    chip8.LoadROM(ROM);

    // Specify the bytes occupied by a single row of display (size of one pixel multiplied by Width)
    int videoPitch = sizeof(chip8.screen[0]) * VIDEO_WIDTH;

    auto lastCycleTime = chrono::high_resolution_clock::now(); // Starting time
    bool quit = false; // variable to check if the exit condition is true

    try{
        // Run the emulation cycles in loop until exit condition becomes true
        while(!quit)
        {
            // Register key input
            quit = screen.ProcessInput(chip8.keypad);

            auto currentTime = chrono::high_resolution_clock::now();

            // Variable to store elapsed time between last cycle and this one
            float dt = chrono::duration<float, chrono::milliseconds::period>(currentTime - lastCycleTime).count();

            if (dt > cycleDelay)
            {
                // Update the last cycle time to current time
                lastCycleTime = currentTime;

                // Execute the emulation cycle
                chip8.Cycle();

                // Update the display
                screen.Update(chip8.screen, videoPitch);
            }
        }
    }
    catch (const char* e)
    {   
        cout<<"AN ERROR OCCURRED !!!!!!!!!!!!"<<endl;
        cout << e << endl;
    }


    return 0;
}

// Path: source-code/Chip8.cpp