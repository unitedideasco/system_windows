import Cocoa
import FlutterMacOS

public class SystemWindowsPlugin: NSObject, FlutterPlugin {

    public static func register(with registrar: FlutterPluginRegistrar) {
        let channel = FlutterMethodChannel(name: "unitedideas.co/system_windows", binaryMessenger: registrar.messenger)
        let instance = SystemWindowsPlugin()
        registrar.addMethodCallDelegate(instance, channel: channel)
    }

    public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
        switch call.method {
            case "getActiveApps":
                result(getActiveApps())
            default:
                result(FlutterMethodNotImplemented)
        }
    }

    private func getActiveApps() -> String {
        var windows = Array<SystemWindow>()
        let ws = NSWorkspace.shared
        let apps = ws.runningApplications
    
        for currentApp in apps {
            if(currentApp.activationPolicy == .regular){
                let systemWindow = SystemWindow(
                name: currentApp.localizedName!, isActive: currentApp.isActive);

                windows.append(systemWindow)
            }
        }
    
    
        do {
            let jsonData = try JSONEncoder().encode(windows)
            let jsonString = String(data: jsonData, encoding: .utf8)!

            return jsonString
        } catch { print(error) }


        return "";
    }
}


struct SystemWindow: Codable {
    
    var name : String;
    var isActive : Bool;
    
}
