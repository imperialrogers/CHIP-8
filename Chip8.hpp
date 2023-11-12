#include <cstdint>
#include <random>
using namespace std;


const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8{
    public:
        Chip8();
        void LoadROM(const char *filename);
        void Cycle();
        
        //////////////////////////////////////////////Instructions Of Chip 8 Emulator//////////////////////////////////////////
        void Table0();
	    void Table8();
	    void TableE();
	    void TableF();

        void OP_NULL(); // Clear the display
        void OP_00E0(); // Clear the display
        void OP_00EE(); // Return from a subroutine
        void OP_1nnn(); // Jump to location nnn
        void OP_2nnn(); // Call subroutine at nnn
        void OP_3xkk(); // Skip next instruction if Vx == kk
        void OP_4xkk(); // Skip next instruction if Vx != kk
        void OP_5xy0(); // Skip next instruction if Vx == Vy
        void OP_6xkk(); // Set Vx = kk
        void OP_7xkk(); // Set Vx = Vx + kk
        void OP_8xy0(); // Set Vx = Vy
        void OP_8xy1(); // Set Vx = Vx OR Vy
        void OP_8xy2(); // Set Vx = Vx AND Vy
        void OP_8xy3(); // Set Vx = Vx XOR Vy
        void OP_8xy4(); // Set Vx = Vx + Vy, set VF = carry
        void OP_8xy5(); // Set Vx = Vx - Vy, set VF = NOT borrow
        void OP_8xy6(); // Set Vx = Vx SHR 1
        void OP_8xy7(); // Set Vx = Vy - Vx, set VF = NOT borrow
        void OP_8xyE(); // Set Vx = Vx SHL 1
        void OP_9xy0(); // Skip next instruction if Vx != Vy
        void OP_Annn(); // Set I = nnn
        void OP_Bnnn(); // Jump to location nnn + V0
        void OP_Cxkk(); // Set Vx = random byte AND kk
        void OP_Dxyn(); // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        void OP_Ex9E(); // Skip next instruction if key with the value of Vx is pressed
        void OP_ExA1(); // Skip next instruction if key with the value of Vx is not pressed
        void OP_Fx07(); // Set Vx = delay timer value
        void OP_Fx0A(); // Wait for a key press, store the value of the key in Vx
        void OP_Fx15(); // Set delay timer = Vx
        void OP_Fx18(); // Set sound timer = Vx
        void OP_Fx1E(); // Set I = I + Vx
        void OP_Fx29(); // Set I = location of sprite for digit Vx
        void OP_Fx33(); // Store BCD representation of Vx in memory locations I, I+1, and I+2
        void OP_Fx55(); // Store registers V0 through Vx in memory starting at location I
        void OP_Fx65(); // Read registers V0 through Vx from memory starting at location I



        //////////////////////////////////////////////Components Of Chip 8 Emulator//////////////////////////////////////////

        uint8_t registers[16]{}; // 16 8-bit registers
        uint8_t memory[4096]{}; // 4K memory
        uint16_t index{}; // Index register
        uint16_t pc{}; // Program counter
        uint16_t stack[16]{}; // Stack for order of execution of calls
        uint8_t sp{}; // Stack Pointer
        uint8_t delayTimer{}; // Delay timer
        uint8_t soundTimer{}; // Sound timer
        uint8_t keypad[16]{}; // Hexadecimal Keypad for user control
        uint32_t video[64 * 32]{}; // Display screen of 64 pixels x 32 pixels
        uint16_t opcode{}; // Current OpCode of the program

        //Initializing Variables
        default_random_engine randGen; // Random number generator
        uniform_int_distribution<uint8_t> randByte; // Random byte

        //Function Pointers
        typedef void (Chip8::*Chip8Func)();
	    Chip8Func table[0xF + 1];
	    Chip8Func table0[0xE + 1];
	    Chip8Func table8[0xE + 1];
	    Chip8Func tableE[0xE + 1];
	    Chip8Func tableF[0x65 + 1];
};