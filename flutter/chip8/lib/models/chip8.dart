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

  _0x0_CLEAR_OR_RETURN(int opcode) {
    int nn = OpCode.NN(opcode);
    if (nn == 0xE0) {
      this._0x00E0_CLEAR_SCREEN(opcode);
    } else if (nn == 0xEE) {
      this._0x00E0_RETRUN_FROM_SUBROUTINE();
    }
  }

  _0x00E0_CLEAR_SCREEN(int opcode) {
    this.memory.vram.reset();
  }

  _0x00E0_RETRUN_FROM_SUBROUTINE() {
    this.pc = this.stack.pop();
  }

  _0x1_JUMP(int opcode) {
    pc = OpCode.NNN(opcode);
  }

  _0x2_CALL_SUBROUTINE(int opcode) {
    this.stack.push(this.pc);
    this.pc = OpCode.NNN(opcode);
  }

  _0x3_SKIP_IF_NN(int opcode) {
    if (this.registers[OpCode.X(opcode)] == OpCode.NN(opcode)) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  _0x4_SKIP_IF_NN(int opcode) {
    if (this.registers[OpCode.X(opcode)] != OpCode.NN(opcode)) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  _0x5_SKIP_IF_XY(int opcode) {
    if (this.registers[OpCode.X(opcode)] == this.registers[OpCode.Y(opcode)]) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  _0x6_SET_X_TO_NN(int opcode) {
    this.registers[OpCode.X(opcode)] = OpCode.NN(opcode);
  }

  _0x7XNN_ADD_NN_TO_X(int opcode) {
    this.registers[OpCode.X(opcode)] += (OpCode.NN(opcode) as num).round();
  }

  _0x8_MATH(int opcode) {
    int n = OpCode.N(opcode);
    if (this.MathOpCodes.containsKey(n)) {
      this.MathOpCodes[n]!(opcode);
    } else {
      print("F Not Implemented");
    }
  }

  _0x8XY0_SET_X_TO_Y(int opcode) {
    this.registers[OpCode.X(opcode)] = this.registers[OpCode.Y(opcode)];
  }

  _0x8XY1_SET_X_TO_OR_Y(int opcode) {
    this.registers[OpCode.X(opcode)] |= this.registers[OpCode.Y(opcode)];
  }

  _0x8XY2_SET_X_TO_AND_Y(int opcode) {
    this.registers[OpCode.X(opcode)] &= this.registers[OpCode.Y(opcode)];
  }

  _0x8XY3_SET_X_TO_XOR_Y(int opcode) {
    this.registers[OpCode.X(opcode)] ^= this.registers[OpCode.Y(opcode)];
  }

  _0x8XY4_ADD_X_TO_Y_CARRY(int opcode) {
    int sum =
        this.registers[OpCode.X(opcode)] + this.registers[OpCode.Y(opcode)];
    // Sum bigger than 0xFF (256) --> Carry
    this.registers[0xF] = sum > 0xFF ? 1 : 0;

    this.registers[OpCode.X(opcode)] += this.registers[OpCode.Y(opcode)];
  }

  _0x8XY5_SUB_Y_FROM_X_CARRY(int opcode) {
    // Sum bigger than 0xFF (256) --> Carry
    if (this.registers[OpCode.X(opcode)] > this.registers[OpCode.Y(opcode)]) {
      this.registers[0xF] = 1;
    } else {
      this.registers[0xF] = 0;
    }

    this.registers[OpCode.X(opcode)] -= this.registers[OpCode.Y(opcode)];
  }

  _0x8XY6_SHIFTR_X_CARRY(int opcode) {
    this.registers[0xF] = this.registers[OpCode.X(opcode)] & 0x01;
    this.registers[OpCode.X(opcode)] >>= 1;
  }

  _0x8XY7_SET_X_TO_Y_MINUS_X_CARRY(int opcode) {
    // Bitmask for last bit
    if (this.registers[OpCode.Y(opcode)] > this.registers[OpCode.X(opcode)]) {
      this.registers[0xF] = 1;
    } else {
      this.registers[0xF] = 0;
    }

    this.registers[OpCode.X(opcode)] =
        this.registers[OpCode.Y(opcode)] - this.registers[OpCode.X(opcode)];
  }

  _0x8XYE_SHIFTL_X_CARRY(int opcode) {
    // Bitmask for checking first bits
    if (this.registers[OpCode.X(opcode)] & 0x80 != 0) {
      this.registers[0xF] = 1;
    } else {
      this.registers[0xF] = 0;
    }

    this.registers[OpCode.X(opcode)] = this.registers[OpCode.X(opcode)] << 1;
  }

  _0x9XY0_SKIP_IF_X_NEQ_Y(int opcode) {
    if (this.registers[OpCode.X(opcode)] != this.registers[OpCode.Y(opcode)]) {
      //Skip next instruction
      this.pc += 2;
    }
  }

  _0xA_SET_I_TO_NNN(int opcode) {
    this.index = OpCode.NNN(opcode);
  }

  _0xBNNN_JUMP_TO_NNN_PLUSregisters0(int opcode) {
    this.pc = (OpCode.NNN(opcode) + this.registers[0]) & 0xFFF;
  }

  _0xCXNN_SET_X_RANDOM(int opcode) {
    this.registers[OpCode.X(opcode)] =
        this.randGenerator.nextInt(256) & OpCode.NN(opcode);
  }

  _0xDXYN_DRAW_SPRITE(int opcode) {
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

  _0xEX9E_SKIP_IF_KEY_PRESSED(int opcode) {
    int x = OpCode.X(opcode);
    if (this.instructionKeys.contains(this.registers[x])) this.pc += 2;
  }

  _0xEXA1_SKIP_IF_KEY_NOT_PRESSED(int opcode) {
    int x = OpCode.X(opcode);
    if (!this.instructionKeys.contains(this.registers[x])) this.pc += 2;
  }

  _0xE_KEY_SKIP(int opcode) {
    int nn = OpCode.NN(opcode);
    if (nn == 0x9e) {
      _0xEX9E_SKIP_IF_KEY_PRESSED(opcode);
    } else if (nn == 0xA1) {
      _0xEXA1_SKIP_IF_KEY_NOT_PRESSED(opcode);
    }
  }

  _0xF_ETC(int opcode) {
    int nn = OpCode.NN(opcode);
    if (this.OtherOpCodesMap.containsKey(nn)) {
      this.OtherOpCodesMap[nn]!(opcode);
    } else {
      print("F Not Implemented");
    }
  }

  _0xFX07_SET_X_TO_DELAY(int opcode) {
    this.registers[OpCode.X(opcode)] = this.delayTimer;
  }

  _0xFX0A_WAIT_FOR_KEY(int opcode) {
    int x = OpCode.X(opcode);
    if (this.instructionKeys.length != 0) {
      this.registers[x] = instructionKeys.first;
    } else {
      // Jump back
      this.pc -= 2;
    }
  }

  _0xFX15_SET_DELAY_TO_X(int opcode) {
    this.delayTimer = this.registers[OpCode.X(opcode)];
  }

  _0xFX18_SET_SOUND_TO_X(int opcode) {
    this.soundTimer = this.registers[OpCode.X(opcode)];
  }

  _0xFX1E_ADD_X_TO_I(int opcode) {
    this.index = (this.index + this.registers[OpCode.X(opcode)]) & 0xFFF;
  }

  _0xFX29_SET_I_SPRITE(int opcode) {
    int x = OpCode.X(opcode);
    this.index = this.registers[x] * 5;
  }

  _0xFX33_STORE_BCD_IN_MEMORY(int opcode) {
    int val = this.registers[OpCode.X(opcode)];
    this.memory.setMemory(this.index, val ~/ 100);
    this.memory.setMemory(this.index + 1, (val % 100) ~/ 10);
    this.memory.setMemory(this.index + 2, (val % 10));
  }

  _0xFX55_STORE_MEMORY(int opcode) {
    int x = OpCode.X(opcode);
    for (int i = 0; i <= x; i++) {
      this.memory.setMemory(this.index + i, this.registers[i]);
    }
  }

  _0xFX65_FILLregisters(int opcode) {
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
      0x0: _0x0_CLEAR_OR_RETURN,
      0x1: _0x1_JUMP,
      0x00E0: _0x00E0_CLEAR_SCREEN,
      0x2: _0x2_CALL_SUBROUTINE,
      0x3: _0x3_SKIP_IF_NN,
      0x4: _0x4_SKIP_IF_NN,
      0x5: _0x5_SKIP_IF_XY,
      0x6: _0x6_SET_X_TO_NN,
      0x7: _0x7XNN_ADD_NN_TO_X,
      0xA: _0xA_SET_I_TO_NNN,
      0xB: _0xBNNN_JUMP_TO_NNN_PLUSregisters0,
      0xC: _0xCXNN_SET_X_RANDOM,
      0xD: _0xDXYN_DRAW_SPRITE,
      0xE: _0xE_KEY_SKIP,
      0xF: _0xF_ETC,
      0x8: _0x8_MATH,
      0x9: _0x9XY0_SKIP_IF_X_NEQ_Y
    };

    OtherOpCodesMap = <int, OPCODE_FUNTION>{
      0x07: _0xFX07_SET_X_TO_DELAY,
      0x0A: _0xFX0A_WAIT_FOR_KEY,
      0x15: _0xFX15_SET_DELAY_TO_X,
      0x18: _0xFX18_SET_SOUND_TO_X,
      0x1E: _0xFX1E_ADD_X_TO_I,
      0x29: _0xFX29_SET_I_SPRITE,
      0x33: _0xFX33_STORE_BCD_IN_MEMORY,
      0x55: _0xFX55_STORE_MEMORY,
      0x65: _0xFX65_FILLregisters,
    };
    MathOpCodes = <int, OPCODE_FUNTION>{
      0x0: _0x8XY0_SET_X_TO_Y,
      0x1: _0x8XY1_SET_X_TO_OR_Y,
      0x2: _0x8XY2_SET_X_TO_AND_Y,
      0x3: _0x8XY3_SET_X_TO_XOR_Y,
      0x4: _0x8XY4_ADD_X_TO_Y_CARRY,
      0x5: _0x8XY5_SUB_Y_FROM_X_CARRY,
      0x6: _0x8XY6_SHIFTR_X_CARRY,
      0x7: _0x8XY7_SET_X_TO_Y_MINUS_X_CARRY,
      0xE: _0x8XYE_SHIFTL_X_CARRY,
    };
    debugPrint(opCodesMap.keys.toString());
  }
}
