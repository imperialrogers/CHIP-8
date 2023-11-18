import 'dart:typed_data';
import 'package:chip8/utils/vram.dart';

class Memory {
  late ByteData memory;
  VRAM vram = VRAM();

  Memory() {
    this.memory = ByteData.view(Uint8List(4096).buffer);
  }

  setPixel(int x, int y, bool value) {
    this.vram.setPixel(x, y, value);
  }

  setMemory(int offset, int data) {
    this.memory.setUint8(offset, data);
  }

  getMemory(int pos) {
    return this.memory.getUint8(pos);
  }

  get screen {
    return this.vram;
  }
}
