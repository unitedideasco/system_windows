import 'dart:async';
import 'dart:convert';
import 'dart:io';

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
    var cleanActiveAppsJson = activeAppsJson?.replaceAll('쳌', '');

    final jsonParts = cleanActiveAppsJson!.split('');

    var correctJson = '';

    for (var i = 0; i < jsonParts.length; i++) {
      final currentChar = jsonParts[i];

      if (currentChar == r'\') {
        final nextChar = jsonParts[i + 1];
        final isNextCharEscapable = nextChar == r'n' || nextChar == r'"';

        if (!isNextCharEscapable) {
          correctJson += currentChar + r'\';
          continue;
        }
      }
      correctJson += currentChar;
    }

    final activeWindowsMap = jsonDecode(correctJson) as List;

    return activeWindowsMap
        .map((windowMap) => SystemWindow.fromJson(windowMap))
        .toList();
  }

  Future<bool> hasScreenRecordingPermission() async {
    if (Platform.isMacOS) {
      final hasScreenRecordingPermission = await _channel.invokeMethod<bool>(
        'hasScreenRecordingPermission',
        <String, Object>{},
      );

      return hasScreenRecordingPermission ?? false;
    } else {
      throw UnimplementedError(
          'hasScreenRecordingPermission() is only supported on MacOS');
    }
  }
}
