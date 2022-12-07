import Cocoa
import FlutterMacOS
import AppKit

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
            case "hasScreenRecordingPermission":
                result(hasScreenRecordingPermission())
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
                
                var title = ""
                
                let visibleWindows = CGWindowListCopyWindowInfo([.optionOnScreenOnly, .excludeDesktopElements], kCGNullWindowID) as! [[String: Any]]
                   
                for window in visibleWindows {
                    let windowOwnerPID = window[kCGWindowOwnerPID as String] as! pid_t
                    
                    if(windowOwnerPID == currentApp.processIdentifier) {
                        title =  window[kCGWindowName as String] as? String ?? ""
                    }
                }
                
                let systemWindow = SystemWindow(
                    name: currentApp.localizedName!,
                    title: title,
                    isActive: currentApp.isActive
                );

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
    
    func hasScreenRecordingPermission() -> Bool {
        CGDisplayStream(
            dispatchQueueDisplay: CGMainDisplayID(),
            outputWidth: 1,
            outputHeight: 1,
            pixelFormat: Int32(kCVPixelFormatType_32BGRA),
            properties: nil,
            queue: DispatchQueue.global(),
            handler: { _, _, _, _ in }
        ) != nil
    }
}

struct SystemWindow: Codable {
    var name : String;
    var title : String;
    var isActive : Bool;
}
