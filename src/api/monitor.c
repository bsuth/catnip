#include "monitor.h"
#include "api/refs.h"
#include "desktop/monitor.h"
#include "user_config/user_config.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>
#include <string.h>
#include <wayland-util.h>

struct api_monitor {
  struct desktop_monitor* desktop_monitor;
  struct wl_listener destroy_listener;
  int index;
  int modes;
};

struct api_monitor_mode {
  struct wlr_output_mode* mode;
};

// Keep track of num_monitors manually. keeps things simpler when manipulating
// Lua's catnip.monitors
static int num_monitors = 0;

// -----------------------------------------------------------------------------
// api_monitor_mode Metatable
// -----------------------------------------------------------------------------

static int
api_monitor_mode__index(lua_State* L)
{
  struct api_monitor_mode* api_monitor_mode = lua_touserdata(L, 1);
  struct wlr_output_mode* mode = api_monitor_mode->mode;

  if (mode == NULL) {
    lua_pushnil(L);
    // TODO: warning?
    return 1;
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "width")) {
    lua_pushnumber(L, mode->width);
  } else if (g_str_equal(field, "height")) {
    lua_pushnumber(L, mode->height);
  } else if (g_str_equal(field, "refresh")) {
    lua_pushnumber(L, mode->refresh);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static const struct luaL_Reg api_monitor_mode_metatable[] = {
  {"__index", api_monitor_mode__index},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// api_monitor Metatable
// -----------------------------------------------------------------------------

static void
api_monitor_push_current_mode(lua_State* L, struct api_monitor* api_monitor)
{
  struct wlr_output_mode* current_mode =
    api_monitor->desktop_monitor->output->current_mode;

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_monitor->modes);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct api_monitor_mode* api_monitor_mode =
      luaL_checkudata(L, -2, "catnip.monitor.mode");

    if (api_monitor_mode->mode == current_mode) {
      lua_pop(L, 1);
      lua_remove(L, -2);
      return;
    }
  }

  lua_pop(L, 1);
  lua_pushnil(L);
}

static int
api_monitor__index(lua_State* L)
{
  struct api_monitor* api_monitor = lua_touserdata(L, 1);
  struct desktop_monitor* desktop_monitor = api_monitor->desktop_monitor;

  if (desktop_monitor == NULL || lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    // TODO: warning?
    return 1;
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "x")) {
    lua_pushnumber(L, desktop_monitor_get_x(desktop_monitor));
  } else if (g_str_equal(field, "y")) {
    lua_pushnumber(L, desktop_monitor_get_y(desktop_monitor));
  } else if (g_str_equal(field, "width")) {
    lua_pushnumber(L, desktop_monitor_get_width(desktop_monitor));
  } else if (g_str_equal(field, "height")) {
    lua_pushnumber(L, desktop_monitor_get_height(desktop_monitor));
  } else if (g_str_equal(field, "refresh")) {
    lua_pushnumber(L, desktop_monitor_get_refresh(desktop_monitor));
  } else if (g_str_equal(field, "mode")) {
    api_monitor_push_current_mode(L, api_monitor);
  } else if (g_str_equal(field, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, api_monitor->modes);
  } else if (g_str_equal(field, "scale")) {
    lua_pushnumber(L, desktop_monitor_get_scale(desktop_monitor));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_monitor__newindex(lua_State* L)
{
  struct api_monitor* api_monitor = lua_touserdata(L, 1);
  struct desktop_monitor* desktop_monitor = api_monitor->desktop_monitor;

  if (desktop_monitor == NULL || lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    return 1; // TODO: error?
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "x")) {
    desktop_monitor_set_x(desktop_monitor, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "y")) {
    desktop_monitor_set_y(desktop_monitor, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "width")) {
    desktop_monitor_set_width(desktop_monitor, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "height")) {
    desktop_monitor_set_height(desktop_monitor, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "refresh")) {
    desktop_monitor_set_refresh(desktop_monitor, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "mode")) {
    struct api_monitor_mode* api_monitor_mode =
      luaL_checkudata(L, 3, "catnip.monitor.mode");
    desktop_monitor_set_mode(desktop_monitor, api_monitor_mode->mode);
  } else if (g_str_equal(field, "scale")) {
    desktop_monitor_set_scale(desktop_monitor, luaL_checknumber(L, 3));
  } else {
    // TODO: error?
  }

  return 0;
}

static int
api_monitor__gc(lua_State* L)
{
  struct api_monitor* api_monitor = lua_touserdata(L, 1);
  wl_list_remove(&api_monitor->destroy_listener.link);
  return 0;
}

static const struct luaL_Reg api_monitor_metatable[] = {
  {"__gc", api_monitor__gc},
  {"__index", api_monitor__index},
  {"__newindex", api_monitor__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// create_api_desktop_monitor
// -----------------------------------------------------------------------------

static void
api_monitor_desktop_monitor_destroy(struct wl_listener* listener, void* data)
{
  struct api_monitor* api_monitor =
    wl_container_of(listener, api_monitor, destroy_listener);

  api_monitor->desktop_monitor = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, api_monitor->modes);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_monitors);

  if (num_monitors == 1) {
    lua_pushnil(L);
    lua_rawseti(L, -2, 1);
  } else {
    // Quick delete by moving last element
    lua_rawgeti(L, -1, num_monitors);

    struct api_monitor* last_api_monitor = lua_touserdata(L, -1);
    last_api_monitor->index = api_monitor->index;

    lua_rawseti(L, -2, api_monitor->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, num_monitors);
  }

  --num_monitors;

  lua_pop(L, 1);
}

void
create_api_desktop_monitor(struct desktop_monitor* desktop_monitor)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_monitors);

  struct api_monitor* api_monitor =
    lua_newuserdata(L, sizeof(struct api_monitor));
  luaL_setmetatable(L, "catnip.monitor");

  ++num_monitors;
  api_monitor->desktop_monitor = desktop_monitor;
  api_monitor->index = num_monitors;

  lua_newtable(L);
  struct wlr_output_mode* mode;
  int mode_counter = 0;
  wl_list_for_each(mode, &desktop_monitor->output->modes, link)
  {
    struct api_monitor_mode* api_monitor_mode =
      lua_newuserdata(L, sizeof(struct api_monitor_mode));
    luaL_setmetatable(L, "catnip.monitor.mode");
    api_monitor_mode->mode = mode;
    lua_rawseti(L, -2, ++mode_counter);
  }
  api_monitor->modes = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_setup_listener(
    &api_monitor->destroy_listener,
    &desktop_monitor->output->events.destroy,
    api_monitor_desktop_monitor_destroy
  );

  lua_rawseti(L, -2, api_monitor->index);

  lua_pop(L, 1);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_api_monitors(lua_State* L)
{
  lua_newtable(L);
  api_catnip_monitors = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_monitors);
  lua_setfield(L, -2, "monitors");
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.monitor");
  luaL_setfuncs(L, api_monitor_metatable, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.monitor.mode");
  luaL_setfuncs(L, api_monitor_mode_metatable, 0);
  lua_pop(L, 1);
}
