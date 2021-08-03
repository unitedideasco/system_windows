//
//  Generated file. Do not edit.
//

#include "generated_plugin_registrant.h"

#include <system_windows/system_windows_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) system_windows_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "SystemWindowsPlugin");
  system_windows_plugin_register_with_registrar(system_windows_registrar);
}
