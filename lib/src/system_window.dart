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
  });

  /// Name that identifies the system window
  final String name;

  /// Title of the window
  final String title;

  /// Is app focused
  final bool isActive;

  factory SystemWindow.fromJson(Map<String, dynamic> json) {
    return SystemWindow(
      name: json['name'],
      title: json['title'],
      isActive: json['isActive'],
    );
  }

  Map<String, String> serialize() {
    return <String, String>{
      "name": this.name,
      "title": this.title,
      "isActive": this.isActive.toString(),
    };
  }

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SystemWindow &&
          name == other.name &&
          title == other.title &&
          isActive == other.isActive;

  @override
  int get hashCode => name.hashCode ^ title.hashCode ^ isActive.hashCode;

  @override
  String toString() {
    return '$runtimeType($name, $title, $isActive)';
  }
}
