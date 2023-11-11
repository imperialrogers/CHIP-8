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

Chip8::Chip8()
    : randGen(chrono::system_clock::now().time_since_epoch().count()) // Initializing random number generator
{
    pc = START_ADDRESS; // Initializing Program Counter to start address

    for(unsigned int i=0; i<FONTSET_SIZE; i++){
        memory[FONTSET_START_ADDRESS + i] = fontset[i]; // Loading the FonSet into memory of Chip 8 Emulator
    }

    randByte=uniform_int_distribution<uint8_t>(0, 255); // Initializing random byte generator from 0 to 255

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


//Implementation of Function Cycle of CHIP 8 class
void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE(){
    --sp; // Decrementing Stack Pointer
    pc=stack[sp]; // Setting Program Counter to the address at the top of the stack
}

void Chip8::OP_1nnn(){
    uint16_t address=opcode & 0x0FFFu;
    pc=address;
}

void Chip8::OP_2nnn(){
    uint16_t address=opcode & 0x0FFFu;

    stack[sp]=pc; // Setting the address of the next instruction to the top of the stack
    sp++; // Incrementing Stack Pointer
    pc=address; // Setting Program Counter to the address of the subroutine
}

void Chip8::OP_3xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    if(registers[Vx] == byte){
        pc += 2;
    }
}

void Chip8::OP_4xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    if(registers[Vx] != byte){
        pc += 2;
    }
}

void Chip8::OP_5xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    if(registers[Vx] != registers[Vy]){
        pc += 2;
    }
}

void Chip8::OP_6xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    registers[Vx] = byte;
}

void Chip8::OP_7xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    registers[Vx] += byte;
}

void Chip8::OP_8xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

	index = address;
}

void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}