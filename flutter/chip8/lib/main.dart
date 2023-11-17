// ignore_for_file: unused_local_variable

import 'dart:async';

import 'package:chip8/decoder/decoder.dart';
import 'package:chip8/models/chip8.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'constants/constants.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Chip-8 Emulator',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.black),
        useMaterial3: true,
      ),
      home: MyHomePage(title: 'C-8'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key? key, this.title}) : super(key: key);

  final String? title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  // ignore: unused_field
  int _counter = 0;
  String _game = "Tetris";
  List<int> _buttonOrder = [
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

  Set<int> _keys = new Set<int>();
  void _incrementCounter() {
    setState(() {
      _counter++;
    });
  }

  Chip8 chip8 = Chip8();
  Image screenImage = new Image.memory(
    BinaryImageDecoder.createImage(
        List.generate(SCREEN_SIZE, (i) => false, growable: false)),
    gaplessPlayback: true,
    filterQuality: FilterQuality.none,
    scale: 0.1,
  );

  Timer _timer = Timer.periodic(Duration(milliseconds: TIMER_SPEED), (_) {});
  @override
  void initState() {
    loadRom();

    super.initState();
  }

  _newTimer([int t = 10]) {
    this._timer.cancel();
    this._timer = new Timer.periodic(new Duration(milliseconds: t), (_) {
      setState(() {
        screenImage = new Image.memory(
          BinaryImageDecoder.createImage(this.chip8.memory.vram.vram),
          gaplessPlayback: true,
          filterQuality: FilterQuality.none,
          scale: 0.1,
        );
      });
    });
  }

  loadRom() {
    // ignore: unnecessary_null_comparison
    if (_timer != null) _timer.cancel();
    final data = rootBundle.load('assets/roms/${_game}.ch8').then((item) {
      var rom = item.buffer.asUint8List();

      this.chip8.loadRom(rom);
      this.chip8.start();
      _timer = new Timer.periodic(new Duration(milliseconds: 10), (_) {
        setState(() {
          screenImage = new Image.memory(
            BinaryImageDecoder.createImage(this.chip8.memory.vram.vram),
            gaplessPlayback: true,
            filterQuality: FilterQuality.none,
            scale: 0.1,
          );
        });
      });
      setState(() {
        screenImage = new Image.memory(
          BinaryImageDecoder.createImage(this.chip8.memory.vram.vram),
          gaplessPlayback: true,
          filterQuality: FilterQuality.none,
          scale: 0.1,
        );
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        toolbarHeight: 70,
        title: Padding(
          padding: const EdgeInsets.symmetric(vertical: 5.0),
          child: Text(
            "CHIP8",
            style: TextStyle(
              fontFamily: "gunplay",
              fontSize: 50,
              color: Colors.white,
            ),
          ),
        ),
        primary: true,
        backgroundColor: Colors.black,
        foregroundColor: Colors.black,
      ),
      body: Container(
        color: Colors.black,
        child: Center(
          child: Column(
            children: [
              Container(
                child: screenImage,
                padding: EdgeInsets.only(top: 10.0, bottom: 20.0),
              ),
              Row(
                children: [
                  Expanded(
                    child: DropdownButtonHideUnderline(
                      child: DropdownButton(
                        dropdownColor: Colors.black,
                        padding: EdgeInsets.only(left: 20),
                        disabledHint: Text(
                          "INSERT ROM",
                          style: TextStyle(
                            color: Colors.white,
                            fontFamily: "gunplay",
                          ),
                        ),
                        icon: Icon(
                          Icons.adf_scanner_outlined,
                        ),
                        hint: Text(
                          "INSERT ROM",
                          style: TextStyle(
                            color: Colors.white,
                            fontFamily: "gunplay",
                          ),
                        ),
                        borderRadius: BorderRadius.circular(10),
                        isExpanded: true,
                        selectedItemBuilder: (context) {
                          return [
                            DropdownMenuItem(
                              child: Text(
                                "INSERT ROM",
                                style: TextStyle(
                                  color: Colors.white,
                                  fontFamily: "gunplay",
                                ),
                              ),
                            ),
                          ];
                        },
                        alignment: Alignment.center,
                        items: List.from(
                          games,
                          growable: false,
                        )
                            .map(
                              (e) => DropdownMenuItem(
                                child: Text(
                                  e,
                                  style: TextStyle(
                                    color: Colors.white,
                                  ),
                                ),
                                value: e,
                              ),
                            )
                            .toList(),
                        onChanged: (dynamic obj) {
                          setState(
                            () {
                              this._game = obj;
                            },
                          );
                          this.chip8.resetCPU();
                          this.chip8.resetMemory();
                          this.loadRom();
                        },
                      ),
                    ),
                  ),
                  Expanded(
                    child: ElevatedButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all<EdgeInsets>(
                          EdgeInsets.all(5.0),
                        ),
                        backgroundColor: MaterialStateProperty.all<Color>(
                          Colors.black,
                        ),
                      ),
                      onPressed: () {
                        this.chip8.stop();
                        this.chip8.resetMemory();
                      },
                      child: Text(
                        "STOP",
                        style: TextStyle(
                          fontFamily: "gunplay",
                          color: Colors.white,
                        ),
                      ),
                    ),
                  ),
                  Expanded(
                    child: ElevatedButton(
                      style: ButtonStyle(
                        padding: MaterialStateProperty.all<EdgeInsets>(
                          EdgeInsets.all(5.0),
                        ),
                        backgroundColor: MaterialStateProperty.all<Color>(
                          Colors.black,
                        ),
                      ),
                      onPressed: () {
                        this.chip8.resetCPU();
                        this.chip8.resetMemory();
                        final data =
                            rootBundle.load('assets/roms/${_game}.ch8').then(
                          (item) {
                            var rom = item.buffer.asUint8List();
                            this.chip8.loadRomAndStart(rom);
                          },
                        );
                      },
                      child: Text(
                        "START",
                        style: TextStyle(
                          fontFamily: "gunplay",
                          color: Colors.white,
                        ),
                      ),
                    ),
                  ),
                ],
              ),
              Expanded(
                child: GridView.count(
                  padding:
                      EdgeInsets.symmetric(vertical: 20.0, horizontal: 10.0),
                  crossAxisCount: 4,
                  children: List.generate(
                    16,
                    (i) {
                      return new Listener(
                        child: Container(
                          margin: const EdgeInsets.all(5.0),
                          decoration: BoxDecoration(
                            color: Colors.black,
                            border: Border.all(
                                style: BorderStyle.solid, color: Colors.white),
                            borderRadius: BorderRadius.circular(10),
                          ),
                          child: Center(
                            child: Text(
                              '${this._buttonOrder[i].toRadixString(16)}',
                              style: TextStyle(
                                fontSize: 30,
                                fontFamily: "gunplay",
                                color: Colors.white,
                              ),
                            ),
                          ),
                        ),
                        onPointerDown: (_) {
                          this._pressKey(this._buttonOrder[i]);
                        },
                        onPointerUp: (_) {
                          this._releaseKey(this._buttonOrder[i]);
                        },
                      );
                    },
                  ),
                ),
              )
            ],
          ),
        ),
      ),
    );
  }

  _pressKey(int key) {
    this._keys.add(key);
    this.chip8.pressKey(key);
  }

  _releaseKey(int key) {
    this._keys.remove(key);
    this.chip8.releaseKey(key);

    Future.delayed(Duration(milliseconds: 1000 ~/ 60), () {});
  }

  _generateButtonList() {
    return Expanded(
      child: GridView.count(
        crossAxisCount: 4,
        children: List.generate(
          16,
          (i) {
            return new GestureDetector(
              child: ElevatedButton(
                style: ButtonStyle(
                  padding: MaterialStateProperty.all<EdgeInsets>(
                    EdgeInsets.all(10.0),
                  ),
                ),
                onPressed: () {},
                child: Text(
                  "${i}",
                  style: TextStyle(
                    fontSize: 60,
                    fontFamily: "gunplay",
                  ),
                ),
              ),
              onTapDown: this._pressKey(i),
              onTapUp: this._releaseKey(i),
            );
          },
        ),
      ),
    );
  }
}
