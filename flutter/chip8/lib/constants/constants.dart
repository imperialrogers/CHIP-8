///////////////////////////////////////////CONSTANTS///////////////////////////////////////////

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;
const int START_ADDRESS = 0x200; // Main code of the program starts at 0x200
const int FONTSET_SIZE =
    80; // Size of Font Set to represent on screen (0-9 and A-F)
const int FONTSET_START_ADDRESS = 0x50; // Font Set starts at 0x50

const CLOCK_SPEED = 1000 ~/ 1000;
const TIMER_SPEED = 1000 ~/ 60;

// Fontset to represent 0-9 and A-F on screen
const List<int> FONTSET = [
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0      e.g.    * * * *   0XF0       *    0x20
  0x20, 0x60, 0x20, 0x20, 0x70, // 1              *     *   0X90     * *    0X60
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2              *     *   0X90       *    0X20
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3              *     *   0X90       *    0X20
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4              *     *   0X90       *    0X20
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5              * * * *   0XF0   * * * *  0XF0
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80 //  F
];

//Games List
const List<String> games = [
  "IBM",
  "15 Puzzle",
  "Airplane",
  "Blinky",
  "Bowling",
  "Breakout",
  "Brick",
  "Cave",
  "CaveExplorer",
  "Coin Flipping",
  "Connect 4",
  "Deflection",
  "Figures",
  "Filter",
  "Fishie",
  "Flight-Runner",
  "GhostEscape",
  "GlitchGhost",
  "Kaleidoscope",
  "Lights-Out",
  "Lunar Lander",
  "Mastermind FourRow",
  "Merlin",
  "Missile",
  "Outlaw",
  "Pong",
  "Pong2.0",
  "Pong2.1",
  "Pong2.2",
  "RL",
  "RockPaperScissors",
  "Rock-Paper-Scissors2",
  "Soccer",
  "Space_Invaders",
  "SpaceJam",
  "Spooky Spot",
  "Squash",
  "Super-Pong",
  "Tank",
  "Tapeworm",
  "test_opcode",
  "Tetris",
  "Tic-Tac-Toe",
  "TombSton",
  "Tron",
  "UFO",
  "Wall",
  "Worm",
  "X-Mirror",
  "ZeroPong"
];

const List<int> buttonOrder = [
  1,
  2,
  3,
  0xC,
  4,
  5,
  6,
  0xD,
  7,
  8,
  9,
  0xE,
  0xA,
  0,
  0xB,
  0xF,
];
