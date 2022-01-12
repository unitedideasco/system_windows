#pragma warning(disable:4458)
#pragma warning(disable:4312)
#pragma warning(disable:5046)



#include "include/system_windows/system_windows_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <winuser.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

struct SystemWindow {
    std::string name;
    bool isActive;
    std::string icon;
};


namespace {




class SystemWindowsPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  SystemWindowsPlugin();

  virtual ~SystemWindowsPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};


struct BITMAP_AND_BYTES {
    Gdiplus::Bitmap* bmp;
    int32_t* bytes;
};


static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
/* 
static BITMAP_AND_BYTES createAlphaChannelBitmapFromIcon(HICON hIcon) {
    // Get the icon info
    ICONINFO iconInfo = {0};
    GetIconInfo(hIcon, &iconInfo);

    // Get the screen DC
    HDC dc = GetDC(NULL);

    // Get icon size info
    BITMAP bm = {0};
    GetObject( iconInfo.hbmColor, sizeof( BITMAP ), &bm );

    // Set up BITMAPINFO
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Extract the color bitmap
    int nBits = bm.bmWidth * bm.bmHeight;
    int32_t* colorBits = new int32_t[nBits];
    GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

    // Check whether the color bitmap has an alpha channel.
        // (On my Windows 7, all file icons I tried have an alpha channel.)
    BOOL hasAlpha = FALSE;
    for (int i = 0; i < nBits; i++) {
        if ((colorBits[i] & 0xff000000) != 0) {
            hasAlpha = TRUE;
            break;
        }
    }

    // If no alpha values available, apply the mask bitmap
    if (!hasAlpha) {
        // Extract the mask bitmap
        int32_t* maskBits = new int32_t[nBits];
        GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
        // Copy the mask alphas into the color bits
        for (int i = 0; i < nBits; i++) {
            if (maskBits[i] == 0) {
                colorBits[i] |= 0xff000000;
            }
        }
        delete[] maskBits;
    } 

    // Release DC and GDI bitmaps
    ReleaseDC(NULL, dc); 
    ::DeleteObject(iconInfo.hbmColor);
    ::DeleteObject(iconInfo.hbmMask); 

    // Create GDI+ Bitmap
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth*4, PixelFormat32bppARGB, (BYTE*)colorBits);
    BITMAP_AND_BYTES ret = {bmp, colorBits};

    return ret;
}


 */
std::string TCHAR2STRING(TCHAR *STR) {
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete chRtn;
	return str;

}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}


// static
void SystemWindowsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "unitedideas.co/system_windows",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<SystemWindowsPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

SystemWindowsPlugin::SystemWindowsPlugin() {}

SystemWindowsPlugin::~SystemWindowsPlugin() {}




BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  DWORD tid, pid;
  const DWORD TITLE_SIZE = 1024;
  WCHAR windowTitle[TITLE_SIZE];

  GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

  int length = ::GetWindowTextLength(hwnd);
  std::wstring title(&windowTitle[0]);

  if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
      return TRUE;
  }

  HWND temp = GetForegroundWindow();

  
  tid = GetWindowThreadProcessId(hwnd, &pid);
  
  TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
  HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );

  if (NULL != hProcess ) {
      HMODULE hMod;
      DWORD cbNeeded;

      if (EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
          GetModuleBaseName( hProcess, hMod, szProcessName,sizeof(szProcessName)/sizeof(TCHAR) );
      }
  }

  CloseHandle( hProcess );


/*   std::vector<std::wstring>& titles =
                              *reinterpret_cast<std::vector<std::wstring>*>(lParam);
  titles.push_back(title); */

  bool isActive = temp == hwnd;
  std::string name = TCHAR2STRING(szProcessName) ;  


  //HICON hIcon=(HICON)GetClassLong(hwnd,-14);



  //std::string base64_icon = base64_encode(&bytes[0], bytes.size());
  SystemWindow window;
  
  window.name = name;
  window.isActive = isActive;
  window.icon = "";

  std::vector<SystemWindow>& windows =
                              *reinterpret_cast<std::vector<SystemWindow>*>(lParam);
  windows.push_back(window);
	return TRUE;
}

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}

void SystemWindowsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {

  if (method_call.method_name().compare("getActiveApps") == 0) {

  	std::vector<SystemWindow> windows;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

    std::string windows_json = "[ ";

    int lastElement = static_cast<int>(windows.size()) - 1;
    for(int i= 0; i < windows.size(); i++) {
             

      std::string currentWindow = "";
        if(i >= lastElement) { 
          currentWindow = "{\"name\": \"" + windows[i].name + "\", \"isActive\": "+BoolToString(windows[i].isActive) +", \"icon\": \"\"}";
        } else {
          currentWindow = "{\"name\": \"" + windows[i].name + "\", \"isActive\": "+BoolToString(windows[i].isActive) +", \"icon\": \"\"},";
        }

      windows_json = windows_json + currentWindow;
      if(i >= lastElement) {
        windows_json = windows_json + " ]";
      }
    }
    result->Success(flutter::EncodableValue(windows_json));
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void SystemWindowsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  SystemWindowsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
