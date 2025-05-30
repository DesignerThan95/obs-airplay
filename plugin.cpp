#include "airplay.hpp"
#include <log/log.hpp>

extern "C" {
#include <obs/obs-module.h>

OBS_DECLARE_MODULE()

OBS_MODULE_USE_DEFAULT_LOCALE("AirPlay", "en-US")

static auto sourceName(void *v) -> const char *
{
  return static_cast<AirPlay *>(v)->name();
}

static auto sourceCreate(obs_data *data, obs_source *obsSource) -> void *
{
  return new AirPlay(data, obsSource);
}

static auto sourceDestroy(void *v) -> void
{
  delete static_cast<AirPlay *>(v);
}

static auto sourceUpdate(void *v, obs_data_t *data) -> void
{
  static_cast<AirPlay *>(v)->update(data);
}

static auto sourceWidth(void *v) -> uint32_t
{
  return static_cast<AirPlay *>(v)->getWidth();
}

static auto sourceHeight(void *v) -> uint32_t
{
  return static_cast<AirPlay *>(v)->getHeight();
}

static auto sourceGetDefaults(obs_data_t *data) -> void
{
  obs_data_set_default_string(data, "server_name", "OBS");
  obs_data_set_default_bool(data, "use_random_mac", true);
  obs_data_set_default_string(data, "mac_address_label", "Configure which MAC Address is being used.");
  obs_data_set_default_string(data, "server_name_info", 
    "Click 'Apply Server Name' to restart the AirPlay server with the new name, or restart OBS to apply changes automatically.");
  obs_data_set_default_string(data, "random_mac_info", 
    "When unchecked, the system's MAC address will be used. Random MAC is recommended to have iOS update the name immediately. iOS is caching the name very aggressively, only a some rare events like a timeout (restart of OBS with opened AirPlay Screensharing dialog) update the name.");
}

static bool apply_settings_clicked(obs_properties_t *props, obs_property_t *property, void *data)
{
  UNUSED_PARAMETER(props);
  UNUSED_PARAMETER(property);
  
  AirPlay *airplay = static_cast<AirPlay *>(data);
  if (airplay)
  {
    airplay->apply_settings();
  }
  return false; // Don't refresh properties
}

static auto sourceGetProperties(void *data) -> obs_properties_t *
{
  obs_properties_t *props = obs_properties_create();
  
  // Server Name section
  obs_properties_add_text(props, "server_name", "Airplay Server Name", OBS_TEXT_DEFAULT);
  obs_properties_add_button(props, "apply_name", "Apply Server Name", apply_settings_clicked);
  obs_properties_add_text(props, "server_name_info", "", OBS_TEXT_INFO);
  
  // Random MAC Address section
  obs_properties_add_text(props, "mac_address_label", "MAC Address Settings", OBS_TEXT_INFO);
  obs_properties_add_bool(props, "use_random_mac", "Use Random MAC Address");
  obs_properties_add_text(props, "random_mac_info", "", OBS_TEXT_INFO);
  
  return props;
}

static struct obs_source_info source = {.id = "AirPlay",
                                        .type = OBS_SOURCE_TYPE_INPUT,
                                        .output_flags = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_AUDIO,
                                        .get_name = sourceName,
                                        .create = sourceCreate,
                                        .destroy = sourceDestroy,
                                        .get_width = sourceWidth,
                                        .get_height = sourceHeight,
                                        .update = sourceUpdate,
                                        .get_defaults = sourceGetDefaults,
                                        .get_properties = sourceGetProperties,
                                        .icon_type = OBS_ICON_TYPE_DESKTOP_CAPTURE};

bool obs_module_load(void)
{
  obs_register_source(&source);
  return true;
}
}
