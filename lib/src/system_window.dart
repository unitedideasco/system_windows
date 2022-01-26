// Copyright 2021 United Ideas. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:convert';
import 'dart:typed_data';

/// Definition of system window
class SystemWindow {
  /// Creates a system window with the given properties.
  SystemWindow({
    required this.name,
    required this.title,
    required this.isActive,
    required this.icon,
  });

  /// Name that identifies the system window
  final String name;

  /// Title of the window
  final String title;

  /// Is app focused
  final bool isActive;

  /// App icon represented by Uint8List
  final Uint8List icon;

  factory SystemWindow.fromJson(Map<String, dynamic> json) {
    return SystemWindow(
      name: json['name'],
      title: json['windowTitle'],
      isActive: json['isActive'],
      icon: Base64Decoder().convert(json['icon']),
    );
  }

  Map<String, String> serialize() {
    return <String, String>{
      "name": this.name,
      "title": this.title,
      "isActive": this.isActive.toString(),
      "icon": Base64Encoder().convert(this.icon),
    };
  }

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SystemWindow &&
          name == other.name &&
          title == other.title &&
          isActive == other.isActive &&
          icon == other.icon;

  @override
  int get hashCode => name.hashCode ^ isActive.hashCode ^ icon.hashCode;

  @override
  String toString() {
    return '$runtimeType($name, $isActive, $icon)';
  }
}
