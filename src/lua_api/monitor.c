#include "monitor.h"
#include "desktop/monitor.h"
#include "user_config/user_config.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>
#include <string.h>
#include <wayland-util.h>

struct lua_monitor {
  struct desktop_monitor* desktop_monitor;
  struct wl_listener destroy_listener;
  int index;
  int modes;
};

struct lua_monitor_mode {
  struct wlr_output_mode* mode;
};

// Keep track of num_monitors manually. keeps things simpler when manipulating
// luas catnip.monitors
static int num_monitors = 0;

// -----------------------------------------------------------------------------
// lua_monitor_mode Metatable
// -----------------------------------------------------------------------------

static int
lua_monitor_mode__index(lua_State* L)
{
  struct lua_monitor_mode* lua_monitor_mode = lua_touserdata(L, 1);
  struct wlr_output_mode* mode = lua_monitor_mode->mode;

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

static const struct luaL_Reg lua_monitor_mode_metatable[] = {
  {"__index", lua_monitor_mode__index},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// lua_monitor Metatable
// -----------------------------------------------------------------------------

static void
lua_monitor_push_current_mode(lua_State* L, struct lua_monitor* lua_monitor)
{
  struct wlr_output_mode* current_mode =
    lua_monitor->desktop_monitor->output->current_mode;

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_monitor->modes);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct lua_monitor_mode* lua_monitor_mode =
      luaL_checkudata(L, -2, "catnip.monitor.mode");

    if (lua_monitor_mode->mode == current_mode) {
      lua_pop(L, 1);
      lua_remove(L, -2);
      return;
    }
  }

  lua_pop(L, 1);
  lua_pushnil(L);
}

static int
lua_monitor__index(lua_State* L)
{
  struct lua_monitor* lua_monitor = lua_touserdata(L, 1);
  struct desktop_monitor* desktop_monitor = lua_monitor->desktop_monitor;

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
    lua_monitor_push_current_mode(L, lua_monitor);
  } else if (g_str_equal(field, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_monitor->modes);
  } else if (g_str_equal(field, "scale")) {
    lua_pushnumber(L, desktop_monitor_get_scale(desktop_monitor));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_monitor__newindex(lua_State* L)
{
  struct lua_monitor* lua_monitor = lua_touserdata(L, 1);
  struct desktop_monitor* desktop_monitor = lua_monitor->desktop_monitor;

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
    struct lua_monitor_mode* lua_monitor_mode =
      luaL_checkudata(L, 3, "catnip.monitor.mode");
    desktop_monitor_set_mode(desktop_monitor, lua_monitor_mode->mode);
  } else if (g_str_equal(field, "scale")) {
    desktop_monitor_set_scale(desktop_monitor, luaL_checknumber(L, 3));
  } else {
    // TODO: error?
  }

  return 0;
}

static int
lua_monitor__gc(lua_State* L)
{
  struct lua_monitor* lua_monitor = lua_touserdata(L, 1);
  wl_list_remove(&lua_monitor->destroy_listener.link);
  return 0;
}

static const struct luaL_Reg lua_monitor_metatable[] = {
  {"__gc", lua_monitor__gc},
  {"__index", lua_monitor__index},
  {"__newindex", lua_monitor__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// create_lua_desktop_monitor
// -----------------------------------------------------------------------------

static void
lua_monitor_desktop_monitor_destroy(struct wl_listener* listener, void* data)
{
  struct lua_monitor* lua_monitor =
    wl_container_of(listener, lua_monitor, destroy_listener);

  lua_monitor->desktop_monitor = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, lua_monitor->modes);

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_getfield(L, -1, "monitors");

  if (num_monitors == 1) {
    lua_pushnil(L);
    lua_rawseti(L, -2, 1);
  } else {
    // Quick delete by moving last element
    lua_rawgeti(L, -1, num_monitors);

    struct lua_monitor* last_lua_monitor = lua_touserdata(L, -1);
    last_lua_monitor->index = lua_monitor->index;

    lua_rawseti(L, -2, lua_monitor->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, num_monitors);
  }

  --num_monitors;

  lua_pop(L, 4);
}

void
create_lua_desktop_monitor(struct desktop_monitor* desktop_monitor)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_getfield(L, -1, "monitors");

  struct lua_monitor* lua_monitor =
    lua_newuserdata(L, sizeof(struct lua_monitor));
  luaL_setmetatable(L, "catnip.monitor");

  ++num_monitors;
  lua_monitor->desktop_monitor = desktop_monitor;
  lua_monitor->index = num_monitors;

  lua_newtable(L);
  struct wlr_output_mode* mode;
  int mode_counter = 0;
  wl_list_for_each(mode, &desktop_monitor->output->modes, link)
  {
    struct lua_monitor_mode* lua_monitor_mode =
      lua_newuserdata(L, sizeof(struct lua_monitor_mode));
    luaL_setmetatable(L, "catnip.monitor.mode");
    lua_monitor_mode->mode = mode;
    lua_rawseti(L, -2, ++mode_counter);
  }
  lua_monitor->modes = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_setup_listener(
    &lua_monitor->destroy_listener,
    &desktop_monitor->output->events.destroy,
    lua_monitor_desktop_monitor_destroy
  );

  lua_rawseti(L, -2, lua_monitor->index);

  lua_pop(L, 4);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

void
init_lua_monitors(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_newtable(L);
  lua_setfield(L, -2, "monitors");
  lua_pop(L, 3);

  luaL_newmetatable(L, "catnip.monitor");
  luaL_setfuncs(L, lua_monitor_metatable, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.monitor.mode");
  luaL_setfuncs(L, lua_monitor_mode_metatable, 0);
  lua_pop(L, 1);
}
