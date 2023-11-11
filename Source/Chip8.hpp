#include <cstdint>
using namespace std;

class Chip8{
    public:
        Chip8();
        void LoadROM(const char *filename);


    private:
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
};