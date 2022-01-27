import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:system_windows/system_windows.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  var systemWindows = SystemWindows();

  var windowsToShow = List<Window>.empty();
  var ticks = 0;

  @override
  void initState() {
    super.initState();
    _init();
  }

  void _init() async {
    Timer.periodic(const Duration(milliseconds: 1000), (timer) async {
      final activeApps = await systemWindows.getActiveApps();

      final wl = activeApps
          .map((w) => Window(w.name, w.title, w.icon, w.isActive, 0, 0))
          .toList();

      if (windowsToShow.isEmpty) {
        windowsToShow = wl;
      }

      for (var element in wl) {
        if (element.isActive) {
          final activeWindow =
              windowsToShow.firstWhere((window) => window.name == element.name);

          activeWindow.previousActivityForce = activeWindow.activityForce;
          activeWindow.activityForce = activeWindow.activityForce + 100;
        }
      }
      setState(() => ticks = ticks + 100);
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: Scaffold(
        body: Container(
          margin: EdgeInsets.symmetric(horizontal: 50.0),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const SizedBox(height: 30.0),
              Text(
                'system_windows',
                style: TextStyle(fontSize: 28.0),
              ),
              const SizedBox(height: 20.0),
              Text(
                'This plugin allowes you to get data about opened apps on your desktop to do cool things just like the example below 🔥',
                style: TextStyle(fontSize: 18.0, color: Colors.black54),
              ),
              const SizedBox(height: 30.0),
              Expanded(
                child: ListView.builder(
                  shrinkWrap: true,
                  itemCount: windowsToShow.length,
                  itemBuilder: (context, index) => Container(
                    margin: EdgeInsets.only(bottom: 20.0),
                    child: Stack(
                      children: [
                        Row(
                          children: [
                            if (windowsToShow[index].icon.isNotEmpty)
                              Image.memory(windowsToShow[index].icon),
                            const SizedBox(width: 12.0),
                            TweenAnimationBuilder<int>(
                              tween: IntTween(
                                begin:
                                    windowsToShow[index].previousActivityForce,
                                end: windowsToShow[index].activityForce,
                              ),
                              duration: Duration(milliseconds: 1000),
                              builder: (context, value, widget) => Expanded(
                                flex: value,
                                child: Container(
                                  decoration: BoxDecoration(
                                    color: Colors.red,
                                    borderRadius: BorderRadius.circular(4.0),
                                  ),
                                  height: 30.0,
                                ),
                              ),
                            ),
                            TweenAnimationBuilder<int>(
                              tween: IntTween(
                                begin: windowsToShow[index].activityForce,
                                end: ticks - windowsToShow[index].activityForce,
                              ),
                              duration: Duration(milliseconds: 1000),
                              builder: (context, value, widget) => Expanded(
                                flex: value,
                                child: Container(
                                  height: 30,
                                ),
                              ),
                            ),
                          ],
                        ),
                        Padding(
                          padding: EdgeInsets.only(left: 55.0, top: 7.0),
                          child: Text(windowsToShow[index].name),
                        ),
                      ],
                    ),
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class Window {
  Window(
    this.name,
    this.title,
    this.icon,
    this.isActive,
    this.activityForce,
    this.previousActivityForce,
  );

  String name;
  String title;
  Uint8List icon;
  bool isActive;
  int activityForce;
  int previousActivityForce;
}
