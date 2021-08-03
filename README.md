# system_windows

A [Flutter package](https://github.com/unitedideasco/system_windows.git) that that enables support for system tray menu for desktop flutter apps.

system_windows is a simple flutter package for fetching list of opened windows on your desktop. Works with MacOS, Windows and Linux.

Icons of opened apps are only supported by MacOS.

# 🤔 How to use it?

Install the package using `pubspec.yaml` and then use it this way: 
```dart

var systemWindows = SystemWindows();


final activeApps = await systemWindows.getActiveApps();

```
And that's it 🎉


Check out the example to see how we built the app that measures what window is most used by user 🔥

![Screenshot of the example](https://github.com/unitedideasco/system_windows/blob/main/res/ss_1.png?raw=true "system_windows_example")


As simple as that. Feel free to contribute :)

Created with ❤️ by United Ideas Flutter Team 