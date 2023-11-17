#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include "Chip8.hpp"
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

void Chip8::dissembler(){
    uint8_t searchNibble = opcode >> 12;
    uint8_t searchN = opcode&0x000Fu;
    uint8_t searchNN = opcode&0x00FFu;
    uint8_t searchNNN = opcode&0x0FFFu;

    switch(searchNibble){
        case 0x0:
        {
            switch (searchNNN) {
                case 0xE0:{
                    (*this).OPCODE_00E0();
                    break;
                }
                case 0xEE:{
                    (*this).OPCODE_00EE();
                    break;
                }
                default:{
                    (*this).OPCODE_NULL();
                    break;
                }
            }
            break;
        }
        case 0x1:
            (*this).OPCODE_1nnn();
            break;
        case 0x2:
            (*this).OPCODE_2nnn();
            break;
        case 0x3:
            (*this).OPCODE_3xkk();
            break;
        case 0x4:
            (*this).OPCODE_4xkk();
            break;
        case 0x5:
            OPCODE_5xy0();
            break;
        case 0x6:
            (*this).OPCODE_6xkk();  
            break;
        case 0x7:
            (*this).OPCODE_7xkk();
            break;
        case 0x8:{
            switch(searchN){
                case 0x0:
                    (*this).OPCODE_8xy0();
                    break;
                case 0x1:
                    (*this).OPCODE_8xy1();
                    break;
                case 0x2:
                    (*this).OPCODE_8xy2();
                    break;
                case 0x3:
                    (*this).OPCODE_8xy3();
                    break;
                case 0x4:
                    (*this).OPCODE_8xy4();
                    break;
                case 0x5:
                    (*this).OPCODE_8xy5();
                    break;
                case 0x6:
                    (*this).OPCODE_8xy6();
                    break;
                case 0x7:
                    (*this).OPCODE_8xy7();
                    break;
                case 0xE:
                    (*this).OPCODE_8xyE();
                    break;
            }
            break;
        }
        case 0x9:
            (*this).OPCODE_9xy0();
            break;
        case 0xA:
            (*this).OPCODE_Annn();  
            break;
        case 0xB:
            (*this).OPCODE_Bnnn();
            break;
        case 0xC:
            (*this).OPCODE_Cxkk();
            break;
        case 0xD:
            (*this).OPCODE_Dxyn();
            break;
        case 0x0E:{
            switch(searchNN){
                case 0x9E:
                    (*this).OPCODE_Ex9E();
                    break;
                case 0xA1:
                    (*this).OPCODE_ExA1();
                    break;
            }
            break;
        }
        case 0xF:{
            switch(searchNN){
                case 0x07:
                    (*this).OPCODE_Fx07();
                    break;
                case 0x0A:
                    (*this).OPCODE_Fx0A();
                    break;
                case 0x15:
                    (*this).OPCODE_Fx15();
                    break;
                case 0x18:
                    (*this).OPCODE_Fx18();
                    break;
                case 0x1E:
                    (*this).OPCODE_Fx1E();
                    break;
                case 0x29:
                    (*this).OPCODE_Fx29();
                    break;
                case 0x33:
                    (*this).OPCODE_Fx33();
                    break;
                case 0x55:
                    (*this).OPCODE_Fx55();
                    break;
                case 0x65:
                    (*this).OPCODE_Fx65();
                    break;
            }
            break;
        }
        default:
            cout<<"UNIMPLEMENTED"<<endl;
            break;
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

void Chip8::Cycle(){
    //Fetching the OpCode of Chip 8 (16 bits)
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc+=2; // Incrementing Program Counter

    (*this).dissembler();

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

void Chip8::OPCODE_NULL()
{}

void Chip8::OPCODE_00E0()
{
	memset(screen, 0, sizeof(screen));
}

void Chip8::OPCODE_00EE(){
    --sp; // Decrementing Stack Pointer
    pc=stack[sp]; // Setting Program Counter to the address at the top of the stack
}

void Chip8::OPCODE_1nnn(){
    uint16_t address=opcode & 0x0FFFu; // Getting the address from the opcode
    pc=address; // Setting Program Counter to the address
}

void Chip8::OPCODE_2nnn(){
    uint16_t address=opcode & 0x0FFFu;

    stack[sp]=pc; // Setting the address of the next instruction to the top of the stack
    sp++; // Incrementing Stack Pointer
    pc=address; // Setting Program Counter to the address of the subroutine
}

void Chip8::OPCODE_3xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    if(registers[Vx] == byte){
        pc += 2;
    }
}

void Chip8::OPCODE_4xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    if(registers[Vx] != byte){
        pc += 2;
    }
}

void Chip8::OPCODE_5xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    if(registers[Vx] != registers[Vy]){
        pc += 2;
    }
}

void Chip8::OPCODE_6xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    registers[Vx] = byte;
}

void Chip8::OPCODE_7xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t byte = opcode & 0x00FFu; // Getting the byte

    registers[Vx] += byte;
}

void Chip8::OPCODE_8xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] = registers[Vy];
}

void Chip8::OPCODE_8xy1(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] |= registers[Vy];
}

void Chip8::OPCODE_8xy2(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] &= registers[Vy];
}

void Chip8::OPCODE_8xy3(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // Getting the register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // Getting the Vy

    registers[Vx] ^= registers[Vy];
}

void Chip8::OPCODE_8xy4()
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

void Chip8::OPCODE_8xy5()
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

void Chip8::OPCODE_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

void Chip8::OPCODE_8xy7()
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

void Chip8::OPCODE_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

void Chip8::OPCODE_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OPCODE_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

	index = address;
}

void Chip8::OPCODE_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}

void Chip8::OPCODE_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OPCODE_Dxyn()
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
			uint32_t* screenPixel = &screen[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

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

void Chip8::OPCODE_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OPCODE_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OPCODE_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;
}

void Chip8::OPCODE_Fx0A()
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

void Chip8::OPCODE_Fx15()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		delayTimer = registers[Vx];
	}

	void Chip8::OPCODE_Fx18()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		soundTimer = registers[Vx];
	}

	void Chip8::OPCODE_Fx1E()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		index += registers[Vx];
	}

	void Chip8::OPCODE_Fx29()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;
		uint8_t digit = registers[Vx];

		index = FONTSET_START_ADDRESS + (5 * digit);
	}

	void Chip8::OPCODE_Fx33()
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

	void Chip8::OPCODE_Fx55()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (uint8_t i = 0; i <= Vx; ++i)
		{
			memory[index + i] = registers[i];
		}
	}

	void Chip8::OPCODE_Fx65()
	{
		uint8_t Vx = (opcode & 0x0F00u) >> 8u;

		for (uint8_t i = 0; i <= Vx; ++i)
		{
			registers[i] = memory[index + i];
		}
	}
