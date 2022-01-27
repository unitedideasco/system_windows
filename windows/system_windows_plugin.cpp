#pragma warning(disable:4458)
#pragma warning(disable:4312)
#pragma warning(disable:5046)
#pragma warning(disable:4996)

#include "include/system_windows/system_windows_plugin.h"
#include <windows.h>
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
#include <locale>  
#include <codecvt>

#pragma comment(lib,"gdiplus.lib")

struct SystemWindow {
    std::string name;
    std::string title;
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

  WCHAR windowName[TITLE_SIZE];
  WCHAR windowTitle[TITLE_SIZE];

  GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

  int length = ::GetWindowTextLength(hwnd);

  std::wstring name(&windowName[0]);
  std::wstring title(&windowTitle[0]);


  if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
      return TRUE;
  }

  HWND temp = GetForegroundWindow();

  
  tid = GetWindowThreadProcessId(hwnd, &pid);
  
  TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");


  int cTxtLen;
  LPTSTR pszMem = new TCHAR[1000];

  HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );

  if (NULL != hProcess ) {
      HMODULE hMod;
      DWORD cbNeeded;

      if (EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
          GetModuleBaseName( hProcess, hMod, szProcessName,sizeof(szProcessName)/sizeof(TCHAR) );

          GetModuleBaseNameW( hProcess, hMod, windowName,sizeof(windowName)/sizeof(WCHAR)  );

          cTxtLen = GetWindowTextLength(hwnd);
          GetWindowText(hwnd, pszMem,cTxtLen + 1);
      }
  }

  CloseHandle( hProcess );

  bool isActive = temp == hwnd;
 // std::string name = TCHAR2STRING(szProcessName) ; 
  std::string winTitle = TCHAR2STRING(pszMem) ;  


  //create wstring to string converter
  std::wstring string_to_convert;
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;

  std::string string_name = converter.to_bytes( name );
  std::string string_title = converter.to_bytes( title );


  SystemWindow window;
  
  window.name = string_name;
  window.isActive = isActive;
  window.icon = "";
  window.title = string_title;

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
          currentWindow = "{\"name\": \"" + windows[i].name + "\", \"title\": \"" + windows[i].title + "\", \"isActive\": "+BoolToString(windows[i].isActive) +", \"icon\": \"\"}";
        } else {
          currentWindow = "{\"name\": \"" + windows[i].name + "\", \"title\": \""+ windows[i].title + "\", \"isActive\": "+BoolToString(windows[i].isActive) +", \"icon\": \"\"},";
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







