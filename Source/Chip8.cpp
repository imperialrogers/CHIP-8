#include "Chip8.hpp"
#include <fstream>
#include <chrono>
#include <random>

using namespace std;

const unsigned int START_ADDRESS=0x200; // Main code of the program starts at 0x200
const unsigned int FONTSET_SIZE=80; // Size of Font Set to represent on screen (0-9 and A-F)
const unsigned int FONTSET_START_ADDRESS=0x50; // Font Set starts at 0x50

// Fontset to represent 0-9 and A-F on screen
uint8_t fontset[FONTSET_SIZE]= {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

Chip8::Chip8(){
    pc = START_ADDRESS; // Initializing Program Counter to start address

    for(unsigned int i=0; i<FONTSET_SIZE; i++){
        memory[FONTSET_START_ADDRESS + i] = fontset[i]; // Loading the FonSet into memory of Chip 8 Emulator
    }
}

//Implementation of Function ROM of CHIP 8 class
void Chip8::LoadROM(char const* filename){
    // Open the file as a stream of binary and move the file pointer to the end
    ifstream file(filename, ios::binary | ios::ate);

    if(file.is_open()){
        // Get size of file and allocate a buffer to hold the contents(contents are in 0's and 1's)
        streampos size = file.tellg(); // Get size of file
        char* buffer = new char[size]; // Allocate a buffer to hold contents of file

        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, ios::beg); // Go to beginning of file
        file.read(buffer, size); // Read Contens of file in buffer
        file.close(); // Closing the File

        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for(long long i=0; i<size; i++){
            memory[START_ADDRESS + i] = buffer[i]; // Loading the contents from temporary buffer into memory of Chip 8
        }

        delete[] buffer; // Deleting buffer to release memory
    }
};