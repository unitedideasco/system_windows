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

      final wl =
          activeApps.map((w) => Window(w.name, w.isActive, 0, 0)).toList();

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
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: ListView.builder(
          itemCount: windowsToShow.length,
          itemBuilder: (context, index) => Stack(
            children: [
              Row(
                children: [
                  TweenAnimationBuilder<int>(
                    tween: IntTween(
                      begin: windowsToShow[index].previousActivityForce,
                      end: windowsToShow[index].activityForce,
                    ),
                    duration: Duration(milliseconds: 1000),
                    builder: (context, value, widget) => Expanded(
                      flex: value,
                      child: Container(
                        height: 30.0,
                        color: Colors.red,
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
              Text(windowsToShow[index].name),
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
    this.isActive,
    this.activityForce,
    this.previousActivityForce,
  );

  String name;
  bool isActive;
  int activityForce;
  int previousActivityForce;
}
