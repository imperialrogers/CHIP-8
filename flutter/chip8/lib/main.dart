import 'dart:async';
import 'package:chip8/decoder/decoder.dart';
import 'package:chip8/models/chip8.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'constants/constants.dart';
import 'package:url_launcher/url_launcher.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await SystemChrome.setPreferredOrientations([
    DeviceOrientation.portraitUp,
  ]);
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
      home: Chip8Screen(title: 'C-8'),
    );
  }
}

class Chip8Screen extends StatefulWidget {
  Chip8Screen({Key? key, this.title}) : super(key: key);

  final String? title;

  @override
  _Chip8ScreenState createState() => _Chip8ScreenState();
}

class _Chip8ScreenState extends State<Chip8Screen> {
  Set<int> inputKeys = new Set<int>();
  String _gameROM = "Tetris";
  List<int> buttonList = [
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
    final data = rootBundle.load('assets/roms/${_gameROM}.ch8').then((item) {
      var ROM = item.buffer.asUint8List();

      this.chip8.loadRom(ROM);
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
    double _screenHeight = MediaQuery.of(context).size.height;
    double _screenWidth = MediaQuery.of(context).size.width;
    return Scaffold(
      appBar: AppBar(
        toolbarHeight: _screenHeight * 0.07,
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
        actions: [
          IconButton(
            icon: Icon(
              Icons.info_outline_rounded,
              color: Colors.white,
            ),
            tooltip: "Help",
            onPressed: _launchURL,
          ),
        ],
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
                              this._gameROM = obj;
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
                        splashFactory: NoSplash.splashFactory,
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
                        splashFactory: NoSplash.splashFactory,
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
                            rootBundle.load('assets/roms/${_gameROM}.ch8').then(
                          (item) {
                            var ROM = item.buffer.asUint8List();
                            this.chip8.loadRomAndStart(ROM);
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
                              '${this.buttonList[i].toRadixString(16)}',
                              style: TextStyle(
                                fontSize: 30,
                                fontFamily: "gunplay",
                                color: Colors.white,
                              ),
                            ),
                          ),
                        ),
                        onPointerDown: (_) {
                          this.pressKey(this.buttonList[i]);
                        },
                        onPointerUp: (_) {
                          this.releaseKey(this.buttonList[i]);
                        },
                      );
                    },
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  pressKey(int key) {
    this.inputKeys.add(key);
    this.chip8.pressKey(key);
  }

  releaseKey(int key) {
    this.inputKeys.remove(key);
    this.chip8.releaseKey(key);

    Future.delayed(Duration(milliseconds: 1000 ~/ 60), () {});
  }

  Buttons() {
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
              onTapDown: this.pressKey(i),
              onTapUp: this.releaseKey(i),
            );
          },
        ),
      ),
    );
  }
}

_launchURL() async {
  final Uri _url = Uri.parse('https://chintanchawda.netlify.app/');
  if (!await launchUrl(_url)) {
    throw Exception('Could not launch $_url');
  }
}
