# system_windows

[system_windows](https://github.com/unitedideasco/system_windows.git) is a simple flutter package for fetching list of opened windows on your desktop. Works with MacOS, Windows and Linux.

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
