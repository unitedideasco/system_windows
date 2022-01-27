import 'dart:async';
import 'dart:convert';

import 'package:flutter/services.dart';
import '../system_windows.dart';

class SystemWindows {
  static const MethodChannel _channel =
      const MethodChannel('unitedideas.co/system_windows');

  Future<List<SystemWindow>> getActiveApps() async {
    final activeAppsJson = await _channel.invokeMethod<String>(
      'getActiveApps',
      <String, Object>{},
    );

    // I don't know why there are a lof of 쳌 chars. This is just bad workaround.
    final cleanActiveAppsJson = activeAppsJson?.replaceAll('쳌', '');
    final activeWindowsMap = jsonDecode(cleanActiveAppsJson!) as List;

    return activeWindowsMap
        .map((windowMap) => SystemWindow.fromJson(windowMap))
        .toList();
  }
}
