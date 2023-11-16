class OpCode {
  static int N(int opcode) {
    return opcode & 0x000F;
  } //Returns the lowest 4 bits (nibble) of the opcode.

  static NN(int opcode) {
    return opcode & 0x00FF;
  } //Returns the lowest 8 bits of the opcode.

  static NNN(int opcode) {
    return opcode & 0x0FFF;
  } //Returns the lowest 12 bits of the opcode.

  static X(int opcode) {
    return (opcode & 0x0F00) >> 8;
  } //Returns the value of the 'X' register, typically represented by bits 8 to 11 (inclusive) of the opcode.

  static Y(int opcode) {
    return (opcode & 0x00F0) >> 4;
  } //Returns the value of the 'Y' register, usually represented by bits 4 to 7 (inclusive) of the opcode.

  static START(int opcode) {
    return opcode >> 12;
  } //Returns the highest 4 bits of the opcode.
}
