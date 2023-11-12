#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
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

    // Function Pointer Table
    //TYPE 1
    table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;


    // TYPE 3
    for (size_t i = 0; i <= 0xE; i++)
	{
		table0[i] = &Chip8::OP_NULL;
		table8[i] = &Chip8::OP_NULL;
		tableE[i] = &Chip8::OP_NULL;
	}
        
	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

    // TYPE 2
	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	for (size_t i = 0; i <= 0x65; i++)
	{
		tableF[i] = &Chip8::OP_NULL;
	}
    
    //TYPE 4
	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;
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

void Chip8::Cycle(){
    //Fetching the OpCode of Chip 8 (16 bits)
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc+=2; // Incrementing Program Counter

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))(); // Decoding and Executing the OpCode

	if (delayTimer > 0)
	{
		--delayTimer; // Decrement the delay timer if it's been set
	}

	if (soundTimer > 0)
	{
		--soundTimer; // Decrement the sound timer if it's been set
	}
}


//Implementation of Function Cycle of CHIP 8 class

void Chip8::Table0()
{
	((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL()
{}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE(){
    --sp; // Decrementing Stack Pointer
    pc=stack[sp]; // Setting Program Counter to the address at the top of the stack
}

void Chip8::OP_1nnn(){
    uint16_t address=opcode & 0x0FFFu; // Getting the address from the opcode
    pc=address; // Setting Program Counter to the address
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
	uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the register Vy

	uint16_t sum = registers[Vx] + registers[Vy]; // Adding Vx and Vy

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

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		delayTimer = registers[Vx];
	}

	void Chip8::OP_Fx18()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		soundTimer = registers[Vx];
	}

	void Chip8::OP_Fx1E()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		index += registers[Vx];
	}

	void Chip8::OP_Fx29()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t digit = registers[Vx];

		index = FONTSET_START_ADDRESS + (5 * digit);
	}

	void Chip8::OP_Fx33()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t value = registers[Vx];

		// Ones-place
		memory[index + 2] = value % 10;
		value /= 10;

		// Tens-place
		memory[index + 1] = value % 10;
		value /= 10;

		// Hundreds-place
		memory[index] = value % 10;
	}

	void Chip8::OP_Fx55()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (uint8_t i = 0; i <= Vx; ++i)
		{
			memory[index + i] = registers[i];
		}
	}

	void Chip8::OP_Fx65()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (uint8_t i = 0; i <= Vx; ++i)
		{
			registers[i] = memory[index + i];
		}
	}
