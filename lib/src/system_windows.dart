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

    final activeWindowsMap = jsonDecode(activeAppsJson!) as List;

    return activeWindowsMap
        .map((windowMap) => SystemWindow(
              name: windowMap['name'],
              isActive: windowMap['isActive'],
            ))
        .toList();
  }
}
