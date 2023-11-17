#include <cstdint>
#include <random>
#include <unordered_map>
using namespace std;

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8{
    public:
        Chip8();
        void LoadROM(const char *filename);
        void Cycle();
        
        //Functions

        void OPCODE_NULL(); // Clear the display
        void OPCODE_00E0(); // Clear the display
        void OPCODE_00EE(); // Return from a subroutine
        void OPCODE_1nnn(); // Jump to location nnn
        void OPCODE_2nnn(); // Call subroutine at nnn
        void OPCODE_3xkk(); // Skip next instruction if Vx == kk
        void OPCODE_4xkk(); // Skip next instruction if Vx != kk
        void OPCODE_5xy0(); // Skip next instruction if Vx == Vy
        void OPCODE_6xkk(); // Set Vx = kk
        void OPCODE_7xkk(); // Set Vx = Vx + kk
        void OPCODE_8xy0(); // Set Vx = Vy
        void OPCODE_8xy1(); // Set Vx = Vx OR Vy
        void OPCODE_8xy2(); // Set Vx = Vx AND Vy
        void OPCODE_8xy3(); // Set Vx = Vx XOR Vy
        void OPCODE_8xy4(); // Set Vx = Vx + Vy, set VF = carry
        void OPCODE_8xy5(); // Set Vx = Vx - Vy, set VF = NOT borrow
        void OPCODE_8xy6(); // Set Vx = Vx SHR 1
        void OPCODE_8xy7(); // Set Vx = Vy - Vx, set VF = NOT borrow
        void OPCODE_8xyE(); // Set Vx = Vx SHL 1
        void OPCODE_9xy0(); // Skip next instruction if Vx != Vy
        void OPCODE_Annn(); // Set I = nnn
        void OPCODE_Bnnn(); // Jump to location nnn + V0
        void OPCODE_Cxkk(); // Set Vx = random byte AND kk
        void OPCODE_Dxyn(); // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        void OPCODE_Ex9E(); // Skip next instruction if key with the value of Vx is pressed
        void OPCODE_ExA1(); // Skip next instruction if key with the value of Vx is not pressed
        void OPCODE_Fx07(); // Set Vx = delay timer value
        void OPCODE_Fx0A(); // Wait for a key press, store the value of the key in Vx
        void OPCODE_Fx15(); // Set delay timer = Vx
        void OPCODE_Fx18(); // Set sound timer = Vx
        void OPCODE_Fx1E(); // Set I = I + Vx
        void OPCODE_Fx29(); // Set I = location of sprite for digit Vx
        void OPCODE_Fx33(); // Store BCD representation of Vx in memory locations I, I+1, and I+2
        void OPCODE_Fx55(); // Store registers V0 through Vx in memory starting at location I
        void OPCODE_Fx65(); // Read registers V0 through Vx from memory starting at location I

        void dissembler();
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
        uint32_t screen[64 * 32]{}; // Display screen of 64 pixels x 32 pixels
        uint16_t opcode{}; // Current OpCode of the program

        //Initializing Variables
        default_random_engine randGen; // Random number generator
        uniform_int_distribution<uint8_t> randByte; // Random byte


};

    // //UNORDERED_MAP
    
    //     table={{0x0, &Chip8::OPCODE_0},
    //         {0x1, &Chip8::OPCODE_1nnn},
    //         {0x2, &Chip8::OPCODE_2nnn},
    //         {0x3, &Chip8::OPCODE_3xkk},
    //         {0x4, &Chip8::OPCODE_4xkk},
    //         {0x5, &Chip8::OPCODE_5xy0},
    //         {0x6, &Chip8::OPCODE_6xkk},
    //         {0x7, &Chip8::OPCODE_7xkk},
    //         {0x8, &Chip8::OPCODE_8},
    //         {0x9, &Chip8::OPCODE_9xy0},
    //         {0xA, &Chip8::OPCODE_Annn},
    //         {0xB, &Chip8::OPCODE_Bnnn},
    //         {0xC, &Chip8::OPCODE_Cxkk},
    //         {0xD, &Chip8::OPCODE_Dxyn},
    //         {0xE, &Chip8::OPCODE_E},
    //         {0xF, &Chip8::OPCODE_F}};

    //     table0={
    //         {0x0, &Chip8::OPCODE_00E0},
    //         {0xE, &Chip8::OPCODE_00EE}
    //     };

    //     table8={
    //         {0x0, &Chip8::OPCODE_8xy0},
    //         {0x1, &Chip8::OPCODE_8xy1},
    //         {0x2, &Chip8::OPCODE_8xy2},
    //         {0x3, &Chip8::OPCODE_8xy3},
    //         {0x4, &Chip8::OPCODE_8xy4},
    //         {0x5, &Chip8::OPCODE_8xy5},
    //         {0x6, &Chip8::OPCODE_8xy6},
    //         {0x7, &Chip8::OPCODE_8xy7},
    //         {0xE, &Chip8::OPCODE_8xyE}
    //     };

    //     tableE={
    //         {0x1, &Chip8::OPCODE_ExA1},
    //         {0xE, &Chip8::OPCODE_Ex9E}
    //     };

    //     tableF={
    //         {0x07, &Chip8::OPCODE_Fx07},
    //         {0x0A, &Chip8::OPCODE_Fx0A},
    //         {0x15, &Chip8::OPCODE_Fx15},
    //         {0x18, &Chip8::OPCODE_Fx18},
    //         {0x1E, &Chip8::OPCODE_Fx1E},
    //         {0x29, &Chip8::OPCODE_Fx29},
    //         {0x33, &Chip8::OPCODE_Fx33},
    //         {0x55, &Chip8::OPCODE_Fx55},
    //         {0x65, &Chip8::OPCODE_Fx65}
    //     };
        // unordered_map<uint8_t, Chip8Func> table;

        // unordered_map<uint8_t, Chip8Func> table0;

        // unordered_map<uint8_t, Chip8Func> table8;

        // unordered_map<uint8_t, Chip8Func> tableE;

        // unordered_map<uint8_t, Chip8Func> tableF;

        // table.insert({0x0, &Chip8::OPCODE_0});
        // table.insert({0x1, &Chip8::OPCODE_1nnn});
        // table.insert({0x2, &Chip8::OPCODE_2nnn});
        // table.insert({0x3, &Chip8::OPCODE_3xkk});
        // table.insert({0x4, &Chip8::OPCODE_4xkk});
        // table.insert({0x5, &Chip8::OPCODE_5xy0});
        // table.insert({0x6, &Chip8::OPCODE_6xkk});
        // table.insert({0x7, &Chip8::OPCODE_7xkk});
        // table.insert({0x8, &Chip8::OPCODE_8});
        // table.insert({0x9, &Chip8::OPCODE_9xy0});
        // table.insert({0xA, &Chip8::OPCODE_Annn});
        // table.insert({0xB, &Chip8::OPCODE_Bnnn});
        // table.insert({0xC, &Chip8::OPCODE_Cxkk});
        // table.insert({0xD, &Chip8::OPCODE_Dxyn});
        // table.insert({0xE, &Chip8::OPCODE_E});
        // table.insert({0xF, &Chip8::OPCODE_F});

        // table0.insert({0x0, &Chip8::OPCODE_00E0});
        // table0.insert({0xE, &Chip8::OPCODE_00EE});

        // table8.insert({0x0, &Chip8::OPCODE_8xy0});
        // table8.insert({0x1, &Chip8::OPCODE_8xy1});
        // table8.insert({0x2, &Chip8::OPCODE_8xy2});
        // table8.insert({0x3, &Chip8::OPCODE_8xy3});
        // table8.insert({0x4, &Chip8::OPCODE_8xy4});
        // table8.insert({0x5, &Chip8::OPCODE_8xy5});
        // table8.insert({0x6, &Chip8::OPCODE_8xy6});
        // table8.insert({0x7, &Chip8::OPCODE_8xy7});
        // table8.insert({0xE, &Chip8::OPCODE_8xyE});

        // tableE.insert({0x1, &Chip8::OPCODE_ExA1});
        // tableE.insert({0xE, &Chip8::OPCODE_Ex9E});
        
        // tableF.insert({0x07, &Chip8::OPCODE_Fx07});
        // tableF.insert({0x0A, &Chip8::OPCODE_Fx0A});
        // tableF.insert({0x15, &Chip8::OPCODE_Fx15});
        // tableF.insert({0x18, &Chip8::OPCODE_Fx18});
        // tableF.insert({0x1E, &Chip8::OPCODE_Fx1E});
        // tableF.insert({0x29, &Chip8::OPCODE_Fx29});
        // tableF.insert({0x33, &Chip8::OPCODE_Fx33});
        // tableF.insert({0x55, &Chip8::OPCODE_Fx55});
        // tableF.insert({0x65, &Chip8::OPCODE_Fx65});