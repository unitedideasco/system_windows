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
                
                var title = ""
                
                let visibleWindows = CGWindowListCopyWindowInfo([.optionOnScreenOnly, .excludeDesktopElements], kCGNullWindowID) as! [[String: Any]]
                   
                
                for window in visibleWindows {
                    let windowOwnerPID = window[kCGWindowOwnerPID as String] as! pid_t
                    
                    if(windowOwnerPID == currentApp.processIdentifier) {
                        title =  window[kCGWindowName as String] as? String ?? ""
                    }
                    
                    print(window[kCGWindowName as String] as! String  + " \(windowOwnerPID)")
                }
                
                let systemWindow = SystemWindow(
                    name: currentApp.localizedName!,title: title, isActive: currentApp.isActive, icon: currentApp.icon!.base64String!);

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
    var title : String;
    var isActive : Bool;
    var icon : String
}




extension NSImage {
    var base64String: String? {
        guard let rep = NSBitmapImageRep(
            bitmapDataPlanes: nil,
            pixelsWide: Int(size.width),
            pixelsHigh: Int(size.height),
            bitsPerSample: 16,
            samplesPerPixel: 4,
            hasAlpha: true,
            isPlanar: false,
            colorSpaceName: .calibratedRGB,
            bytesPerRow: 0,
            bitsPerPixel: 0
            ) else {
                print("Couldn't create bitmap representation")
                return nil
        }

        NSGraphicsContext.saveGraphicsState()
        NSGraphicsContext.current = NSGraphicsContext(bitmapImageRep: rep)
        draw(at: NSZeroPoint, from: NSZeroRect, operation: .sourceOver, fraction: 1.0)
        NSGraphicsContext.restoreGraphicsState()

        guard let data = rep.representation(using: NSBitmapImageRep.FileType.png, properties: [NSBitmapImageRep.PropertyKey.compressionFactor: 1.0]) else {
            print("Couldn't create PNG")
            return nil
        }

        // With prefix
        // return "data:image/png;base64,\(data.base64EncodedString(options: []))"
        // Without prefix
        return data.base64EncodedString(options: [])
    }
}

