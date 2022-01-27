#include "include/system_windows/system_windows_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <iterator>
#include <vector>
#include <string.h>
#define SYSTEM_WINDOWS_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), system_windows_plugin_get_type(), \
                              SystemWindowsPlugin))

struct S {
  const char* value;
};

struct _SystemWindowsPlugin {
  GObject parent_instance;
};

G_DEFINE_TYPE(SystemWindowsPlugin, system_windows_plugin, g_object_get_type())


struct SystemWindow {
    std::string name;
    std::string title;
    bool isActive;
    std::string icon;
};


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

inline const char * const BoolToString(bool b) {
  return b ? "true" : "false";
}


// Called when a method call is received from Flutter.
static void system_windows_plugin_handle_method_call(
    SystemWindowsPlugin* self,
    FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);

  if (strcmp(method, "getActiveApps") == 0) {

    std::string xprop_result = exec("xprop -root | grep '_NET_CLIENT_LIST_STACKING(WINDOW)'");

    std::vector<std::string> window_ids;




    int index = 47; 

    while(index < xprop_result.length()) {
      std::string part = xprop_result.substr(index, 9);

      window_ids.push_back(part);
      index = index+11;
    }

    std::vector<SystemWindow> system_windows;
    SystemWindow system_window;

    for(int i = 0; i < window_ids.size(); i++) {

    
        std::string hex = window_ids[i];
        std::string x_prop_id_command = "xprop -id " + hex + " | grep 'WM_CLASS'";
        std::string xprop_id_result = exec(x_prop_id_command.c_str());
        
        int quoteCount = 0;

        std::string app_name = "";

        for(int j =0; j < xprop_id_result.length(); j++) {

          if(xprop_id_result[j] == '\"') {
            quoteCount++;
          }

          if(quoteCount < 4 && quoteCount > 2) {
            app_name = app_name + xprop_id_result[j];
          }
        }

        std::string x_prop_active_command = "xprop -id " + hex + " | grep '_NET_WM_STATE_FOCUSED'";
        std::string xprop_active_result = exec(x_prop_active_command.c_str());

        std::string x_prop_title_command = "xwininfo -frame -id " + hex + " | grep 'xwininfo'";
        std::string xprop_title_result = exec(x_prop_title_command.c_str());

        
        std::string window_title = "";

        for(int j = 32; j < xprop_title_result.length()-2; j++) {
          window_title = window_title + xprop_title_result[j];
        }

        system_window.title = window_title;

        system_window.name = app_name;
        system_window.isActive = xprop_active_result != "";
        
        /*
        std::cout << " - - - - - - - - - - - - - - - - - - \n";
        std::cout << "App name: " <<  system_window.name << "\n";
        std::cout << "Window title: " << system_window.title << "\n";
        std::cout << "Is focused: " << BoolToString(system_window.isActive)   << "\n";
        */

        system_windows.push_back(system_window); 
      
    } 




    std::string windows_json = "[ ";

    int lastElement = system_windows.size()- 1;
    for(int i= 0; i < system_windows.size(); i++) {
       

      
      std::string currentWindow = "";
        if(i >= lastElement) { 
          currentWindow = "{\"name\": " + system_windows[i].name + "\", \"title\": \"" + system_windows[i].title + " \", \"isActive\": "+BoolToString(system_windows[i].isActive) +", \"icon\": \"\"}";
        } else {
          currentWindow = "{\"name\": " + system_windows[i].name + "\", \"title\": \"" + system_windows[i].title + " \", \"isActive\": "+BoolToString(system_windows[i].isActive) +", \"icon\": \"\"},";
        }

      windows_json = windows_json + currentWindow;
      if(i >= lastElement) {
        windows_json = windows_json + " ]";
      }

    } 
      


    g_autofree gchar *json = g_strdup_printf("%s", windows_json.c_str());
    g_autoptr(FlValue) result = fl_value_new_string(json);


    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}





static void system_windows_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(system_windows_plugin_parent_class)->dispose(object);
}

static void system_windows_plugin_class_init(SystemWindowsPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = system_windows_plugin_dispose;
}

static void system_windows_plugin_init(SystemWindowsPlugin* self) {}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  SystemWindowsPlugin* plugin = SYSTEM_WINDOWS_PLUGIN(user_data);
  system_windows_plugin_handle_method_call(plugin, method_call);
}

void system_windows_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  SystemWindowsPlugin* plugin = SYSTEM_WINDOWS_PLUGIN(
      g_object_new(system_windows_plugin_get_type(), nullptr));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "unitedideas.co/system_windows",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
