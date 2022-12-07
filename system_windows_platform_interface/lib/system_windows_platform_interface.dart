import 'dart:async';

import 'package:flutter/services.dart';

class SystemWindowsPlatformInterface {
  static const MethodChannel _channel =
      const MethodChannel('system_windows_platform_interface');

  static Future<String> get platformVersion async {
    final String version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }
}
