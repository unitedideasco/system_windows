// Copyright 2021 United Ideas. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


/// Definition of system window
class SystemWindow {
  /// Creates a system window with the given properties.
  SystemWindow({
    required this.name,
    required this.isActive,
  });

  /// Name that identifies the system window
  final String name;

  /// Is app focused
  final bool isActive;

  factory SystemWindow.fromJson(Map<String, dynamic> json) {
    return SystemWindow(
      name: json['name'],
      isActive: json['isActive'],
    );
  }

  Map<String, String> serialize() {
    return <String, String>{
      "name": this.name,
      "isActive": this.isActive.toString(),
    };
  }

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SystemWindow &&
          name == other.name &&
          isActive == other.isActive;

  @override
  int get hashCode => name.hashCode ^ isActive.hashCode;

  @override
  String toString() {
    return '$runtimeType($name, $isActive)';
  }
}
