# system_windows

A [Flutter package](https://github.com/unitedideasco/system_windows.git) that that enables support for system tray menu for desktop flutter apps. **Currently working only on MacOS**. 

# Getting Started

Install the package using `pubspec.yaml`
## API

Below we show how to use system_windows

```dart
...

var systemWindows = SystemWindows();

...

final activeApps = await systemWindows.getActiveApps();

```

As simple as that. Feel free to contribute :)