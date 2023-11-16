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

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Chip-8 Emulator',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: MyHomePage(title: 'Chip-8 Emulator'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key? key, this.title}) : super(key: key);

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String? title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;
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
    0xF
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
    this?._timer?.cancel();
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
    if (_timer != null) _timer.cancel();
    final data = rootBundle.load('assets/Pong.ch8').then((item) {
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
        title: Text(widget.title!),
      ),
      body: Center(
          child: Column(
        children: <Widget>[
          screenImage,
          Expanded(
              child: GridView.count(
                  crossAxisCount: 4,
                  children: List.generate(16, (i) {
                    return new Listener(
                      child: Container(
                        decoration: BoxDecoration(
                            color: Colors.grey, border: Border.all()),
                        child: Center(
                            child: Text(
                                '${this._buttonOrder[i].toRadixString(16)}')),
                      ),
                      onPointerDown: (_) {
                        this._pressKey(this._buttonOrder[i]);
                      },
                      onPointerUp: (_) {
                        this._releaseKey(this._buttonOrder[i]);
                      },
                    );
                  })))
        ],
      )),
      // This trailing comma makes auto-formatting nicer for build methods.
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
            children: List.generate(16, (i) {
              return new GestureDetector(
                child: ElevatedButton(onPressed: () {}, child: Text("${i}")),
                onTapDown: this._pressKey(i),
                onTapUp: this._releaseKey(i),
              );
            })));
  }
}
