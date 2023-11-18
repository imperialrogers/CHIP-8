import 'package:chip8/constants/constants.dart';

class VRAM {
  List<bool> vram =
      List.generate(SCREEN_SIZE, (index) => false, growable: false);

  void setPixel(int x, int y, bool val) {
    this.vram[y * 64 + x] = val;
    return;
  }

  bool getPixels(int x, int y) {
    return this.vram[y * SCREEN_WIDTH + x];
  }

  void reset() {
    for (int i = 0; i < SCREEN_SIZE; i++) {
      this.vram[i] = false;
    }
  }
}
