import 'dart:async';
import 'dart:math';
import 'dart:typed_data';

import 'package:chip8/constants/constants.dart';
import 'package:chip8/utils/memory.dart';
import 'package:chip8/utils/opcode.dart';
import 'package:chip8/utils/stack.dart';
import 'package:flutter/material.dart';

typedef OPCODE_FUNTION = Function(int);

class Chip8 {
  //////////////////////////////////////////////Components Of Chip 8 Emulator//////////////////////////////////////////

  Memory memory = new Memory(); // 4K memory
  Uint8List registers = new Uint8List(16); // 16 8-bit registers
  MemoryStack stack = new MemoryStack(); // Stack
  int pc = 0x200; // Program Counter starts at 0x200 in CHIP-8
  int index = 0; // Index register

  //Function Pointer Tables
  Map<int, void Function(int)> opCodesMap = {}; // Map of opcodes
  Map<int, void Function(int)> OtherOpCodesMap = {}; // Map of other opcodes
  Set<int> instructionKeys = new Set<int>(); // Set of keys
  Map<int, OPCODE_FUNTION> MathOpCodes = {};

  //Timers
  int steps = 0; // Number of steps the emulator has taken
  Timer timerClock =
      Timer.periodic(Duration(milliseconds: TIMER_SPEED), (_) {}); // Timer
  Timer mainClock =
      new Timer(Duration(milliseconds: TIMER_SPEED), () {}); // Main Clock Timer

  int delayTimer = 0; // Delay Timer (60Hz)for adjusting time
  int soundTimer = 0; // Sound Timer for adjusting sound
  final randGenerator = new Random(); // Random Number Generator

  //Functions

  void timerStep() {
    if (this.delayTimer > 0) this.delayTimer--;

    if (this.soundTimer > 0) {
      this.soundTimer--;
      if (this.soundTimer == 0) {}
    }
  } // Timer Step that is called every 60Hz

  void executeStep() {
    if (pc >= 4096) {
      pc = 0x200;
    }

    int opcode = memory.memory.getUint16(this.pc);

    final maskedOpcode = OpCode.START(opcode);
    this.printRegisters();
    //  print(opcode.toRadixString(16) + " | " + maskedOpcode.toRadixString(16));
    // print(pc);
    this.pc += 2;

    if (opCodesMap.containsKey(maskedOpcode)) {
      opCodesMap[maskedOpcode]!(opcode);
    } else {
      print("not implemented");
    }
  } // Executes a single step

  clockStep() {
    executeStep();
  } // Clock Step that is called every 500Hz

  void printRegisters() {
    var str = "";
    for (var i = 0; i < this.registers.length; i++) {
      str += i.toString();
      str += ":";
      str += this.registers[i].toRadixString(16);
      str += " | ";
    }
    print(str);
  } // Prints the registers

  void resetCPU() {
    this.pc = 0x0200;
    this.index = 0;
    this.steps = 0;
    this.delayTimer = 0;
    this.soundTimer = 0;
  } // Resets to default value

  step() {
    clockStep();
    if (this.steps == CLOCK_SPEED ~/ TIMER_SPEED) {
      this.timerStep();
    }
    steps++;
  }

  reset() {
    stop();
    this.resetCPU();
    initializeClocks();
    resetMemory();
  }

  loadFont() {
    // ignore: unused_local_variable
    int offset = 0;
    for (int i = 0; i < FONTSET.length; i++) {
      var character = FONTSET[i];
      this.memory.setMemory(i, character);
    }
  }

  resetMemory() {
    this.memory = new Memory();
    this.stack = new MemoryStack();
    loadFont();
  } // Resets the memory buffer

  stop() {
    this.timerClock.cancel();
    this.mainClock.cancel();
  } // Stop the clock execution

  void initializeClocks() {
    stop();

    this.timerClock = Timer.periodic(Duration(milliseconds: TIMER_SPEED), (_) {
      this.timerStep();
    });

    this.mainClock =
        new Timer.periodic(Duration(milliseconds: CLOCK_SPEED), (_) {
      clockStep();
    });
  } // Initialize Clocks with default values

  start() {
    stop();
    this.resetCPU();
    initializeClocks();
  } // Starts the code execution

  void loadRom(Uint8List rom) {
    print('LOADING ROM');
    for (int i = 0; i < rom.length; i++) {
      this.memory.setMemory(START_ADDRESS + i, rom[i]);
    }

    //printing the rom
    // for (var i = 0; i < this.memory.memory.lengthInBytes; i++) {
    //   print(this.memory.getMemory(i));
    // }
  }

  void loadRomAndStart(Uint8List rom) {
    this.loadRom(rom);
    this.start();
  }

  //////////////////////////////////////////////////Game Logic////////////////////////////////////////////////

  OPCODE_0x0(int opcode) {
    int nn = OpCode.NN(opcode);
    if (nn == 0xE0) {
      this.OPCODE_0x00E0(opcode);
    } else if (nn == 0xEE) {
      this.OPCODE_0x00EE();
    }
  }

  OPCODE_0x00E0(int opcode) {
    this.memory.vram.reset();
  }

  OPCODE_0x00EE() {
    this.pc = this.stack.pop();
  }

  OPCODE_0x1(int opcode) {
    pc = OpCode.NNN(opcode);
  }

  OPCODE_0x2(int opcode) {
    this.stack.push(this.pc);
    this.pc = OpCode.NNN(opcode);
  }

  OPCODE_0x3(int opcode) {
    if (this.registers[OpCode.X(opcode)] == OpCode.NN(opcode)) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  OPCODE_0x4(int opcode) {
    if (this.registers[OpCode.X(opcode)] != OpCode.NN(opcode)) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  OPCODE_0x5(int opcode) {
    if (this.registers[OpCode.X(opcode)] == this.registers[OpCode.Y(opcode)]) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  OPCODE_0x6(int opcode) {
    this.registers[OpCode.X(opcode)] = OpCode.NN(opcode);
  }

  OPCODE_0x7(int opcode) {
    this.registers[OpCode.X(opcode)] += (OpCode.NN(opcode) as num).round();
  }

  OPCODE_0x8_MAIN(int opcode) {
    int n = OpCode.N(opcode);
    if (this.MathOpCodes.containsKey(n)) {
      this.MathOpCodes[n]!(opcode);
    } else {
      print("F Not Implemented");
    }
  }

  OPCODE_0x8XY0(int opcode) {
    this.registers[OpCode.X(opcode)] = this.registers[OpCode.Y(opcode)];
  }

  OPCODE_0x8XY1(int opcode) {
    this.registers[OpCode.X(opcode)] |= this.registers[OpCode.Y(opcode)];
  }

  OPCODE_0x8XY2(int opcode) {
    this.registers[OpCode.X(opcode)] &= this.registers[OpCode.Y(opcode)];
  }

  OPCODE_0x8XY3(int opcode) {
    this.registers[OpCode.X(opcode)] ^= this.registers[OpCode.Y(opcode)];
  }

  OPCODE_08XY4(int opcode) {
    int sum =
        this.registers[OpCode.X(opcode)] + this.registers[OpCode.Y(opcode)];
    // Sum bigger than 0xFF (256) --> Carry
    this.registers[0xF] = sum > 0xFF ? 1 : 0;

    this.registers[OpCode.X(opcode)] += this.registers[OpCode.Y(opcode)];
  }

  OPCODE_0x8XY5(int opcode) {
    // Sum bigger than 0xFF (256) --> Carry
    if (this.registers[OpCode.X(opcode)] > this.registers[OpCode.Y(opcode)]) {
      this.registers[0xF] = 1;
    } else {
      this.registers[0xF] = 0;
    }

    this.registers[OpCode.X(opcode)] -= this.registers[OpCode.Y(opcode)];
  }

  OPCODE_0x8XY6(int opcode) {
    this.registers[0xF] = this.registers[OpCode.X(opcode)] & 0x01;
    this.registers[OpCode.X(opcode)] >>= 1;
  }

  OPCODE_0x8XY7(int opcode) {
    // Bitmask for last bit
    if (this.registers[OpCode.Y(opcode)] > this.registers[OpCode.X(opcode)]) {
      this.registers[0xF] = 1;
    } else {
      this.registers[0xF] = 0;
    }

    this.registers[OpCode.X(opcode)] =
        this.registers[OpCode.Y(opcode)] - this.registers[OpCode.X(opcode)];
  }

  OPCODE_0x8XYE(int opcode) {
    // Bitmask for checking first bits
    if (this.registers[OpCode.X(opcode)] & 0x80 != 0) {
      this.registers[0xF] = 1;
    } else {
      this.registers[0xF] = 0;
    }

    this.registers[OpCode.X(opcode)] = this.registers[OpCode.X(opcode)] << 1;
  }

  OPCODE_0x9XY0(int opcode) {
    if (this.registers[OpCode.X(opcode)] != this.registers[OpCode.Y(opcode)]) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  OPCODE_0xANNN(int opcode) {
    this.index = OpCode.NNN(opcode);
  }

  OPCODE_0xBNNN(int opcode) {
    this.pc = (OpCode.NNN(opcode) + this.registers[0]) & 0xFFF;
  }

  OPCODE_0xCNNN(int opcode) {
    this.registers[OpCode.X(opcode)] =
        this.randGenerator.nextInt(256) & OpCode.NN(opcode);
  }

  OPCODE_DXYN(int opcode) {
    int width = 8;
    int height = OpCode.N(opcode);

    int Vx = this.registers[OpCode.X(opcode)];
    int Vy = this.registers[OpCode.Y(opcode)];

    this.registers[0xF] = 0;
    for (int y = 0; y < height; y++) {
      int sprite = this.memory.getMemory(this.index + y);
      int yPos = (Vy + y) % SCREEN_HEIGHT;

      // => 1111 1111, 0111 1111, 0011 1111 ....
      int bitmask = 0x80;
      for (int x = 0; x < width; x++) {
        int xPos = (Vx + x) % SCREEN_WIDTH;

        bool currentPixel = this.memory.vram.getPixels(xPos, yPos);

        bool doDraw = sprite & bitmask > 0;

        if (doDraw && currentPixel) {
          this.registers[0xF] = 1;
          doDraw = false;
        } else if (!doDraw && currentPixel) {
          doDraw = true;
        }

        this.memory.setPixel(xPos, yPos, doDraw);
        bitmask = bitmask >> 1;
      }
    }
  }

  OPCODE_0xEX9E(int opcode) {
    int x = OpCode.X(opcode);
    if (this.instructionKeys.contains(this.registers[x])) this.pc += 2;
  }

  OPCODE_0xEXA1(int opcode) {
    int x = OpCode.X(opcode);
    if (!this.instructionKeys.contains(this.registers[x])) this.pc += 2;
  }

  OPCODE_0xE_MAIN(int opcode) {
    int nn = OpCode.NN(opcode);
    if (nn == 0x9e) {
      OPCODE_0xEX9E(opcode);
    } else if (nn == 0xA1) {
      OPCODE_0xEXA1(opcode);
    }
  }

  OPCODE_0xF_MAIN(int opcode) {
    int nn = OpCode.NN(opcode);
    if (this.OtherOpCodesMap.containsKey(nn)) {
      this.OtherOpCodesMap[nn]!(opcode);
    } else {
      print("F Not Implemented");
    }
  }

  OPCODE_0xFX07(int opcode) {
    this.registers[OpCode.X(opcode)] = this.delayTimer;
  }

  OPCODE_0xFX0A(int opcode) {
    int x = OpCode.X(opcode);
    if (this.instructionKeys.length != 0) {
      this.registers[x] = instructionKeys.first;
    } else {
      // Jump back
      this.pc -= 2;
    }
  }

  OPCODE_0xFX15(int opcode) {
    this.delayTimer = this.registers[OpCode.X(opcode)];
  }

  OPCODE_0xFX18(int opcode) {
    this.soundTimer = this.registers[OpCode.X(opcode)];
  }

  OPCODE_0xFX1E(int opcode) {
    this.index = (this.index + this.registers[OpCode.X(opcode)]) & 0xFFF;
  }

  OPCODE_0xFX29(int opcode) {
    int x = OpCode.X(opcode);
    this.index = this.registers[x] * 5;
  }

  OPCODE_0xFX33(int opcode) {
    int val = this.registers[OpCode.X(opcode)];
    this.memory.setMemory(this.index, val ~/ 100);
    this.memory.setMemory(this.index + 1, (val % 100) ~/ 10);
    this.memory.setMemory(this.index + 2, (val % 10));
  }

  OPCODE_0xFX55(int opcode) {
    int x = OpCode.X(opcode);
    for (int i = 0; i <= x; i++) {
      this.memory.setMemory(this.index + i, this.registers[i]);
    }
  }

  OPCODE_0xFX65(int opcode) {
    int x = OpCode.X(opcode);
    for (int i = 0; i <= x; i++) {
      this.registers[i] = this.memory.getMemory(index + i);
    }
  }

  pressKey(int k) {
    this.instructionKeys.add(k);
  }

  releaseKey(int k) {
    this.instructionKeys.remove(k);
  }

  //////////////////////////////////////////////////Constructor////////////////////////////////////////////////
  Chip8() {
    this.resetCPU();
    resetMemory();

    opCodesMap = <int, OPCODE_FUNTION>{
      0x0: OPCODE_0x0,
      0x1: OPCODE_0x1,
      0x00E0: OPCODE_0x00E0,
      0x2: OPCODE_0x2,
      0x3: OPCODE_0x3,
      0x4: OPCODE_0x4,
      0x5: OPCODE_0x5,
      0x6: OPCODE_0x6,
      0x7: OPCODE_0x7,
      0xA: OPCODE_0xANNN,
      0xB: OPCODE_0xBNNN,
      0xC: OPCODE_0xCNNN,
      0xD: OPCODE_DXYN,
      0xE: OPCODE_0xE_MAIN,
      0xF: OPCODE_0xF_MAIN,
      0x8: OPCODE_0x8_MAIN,
      0x9: OPCODE_0x9XY0
    };

    OtherOpCodesMap = <int, OPCODE_FUNTION>{
      0x07: OPCODE_0xFX07,
      0x0A: OPCODE_0xFX0A,
      0x15: OPCODE_0xFX15,
      0x18: OPCODE_0xFX18,
      0x1E: OPCODE_0xFX1E,
      0x29: OPCODE_0xFX29,
      0x33: OPCODE_0xFX33,
      0x55: OPCODE_0xFX55,
      0x65: OPCODE_0xFX65,
    };
    MathOpCodes = <int, OPCODE_FUNTION>{
      0x0: OPCODE_0x8XY0,
      0x1: OPCODE_0x8XY1,
      0x2: OPCODE_0x8XY2,
      0x3: OPCODE_0x8XY3,
      0x4: OPCODE_08XY4,
      0x5: OPCODE_0x8XY5,
      0x6: OPCODE_0x8XY6,
      0x7: OPCODE_0x8XY7,
      0xE: OPCODE_0x8XYE,
    };
    debugPrint(opCodesMap.keys.toString());
  }
}
