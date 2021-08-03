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
    print(activeAppsJson);

    final activeWindowsMap = jsonDecode(activeAppsJson!) as List;

    return activeWindowsMap
        .map((windowMap) => SystemWindow.fromJson(windowMap))
        .toList();
        return [];
  }
}
