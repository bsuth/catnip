#include "monitor.h"
#include "desktop/monitor.h"
#include "user_config/user_config.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>

struct lua_monitor {
  struct desktop_monitor* desktop_monitor;
  struct wl_listener destroy_listener;
  int index;
};

// Keep track of num_monitors manually. keeps things simpler when manipulating
// luas catnip.monitors
static int num_monitors = 0;

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
lua_monitor__index(lua_State* L)
{
  struct lua_monitor* lua_monitor = lua_touserdata(L, 1);
  struct desktop_monitor* desktop_monitor = lua_monitor->desktop_monitor;

  if (desktop_monitor == NULL || lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
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
    // TODO
  } else if (g_str_equal(field, "modes")) {
    // TODO
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
    // TODO
  } else if (g_str_equal(field, "modes")) {
    // TODO
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

// -----------------------------------------------------------------------------
// create_lua_desktop_monitor
// -----------------------------------------------------------------------------

static void
lua_monitor_desktop_monitor_destroy(struct wl_listener* listener, void* data)
{
  struct lua_monitor* lua_monitor =
    wl_container_of(listener, lua_monitor, destroy_listener);

  lua_monitor->desktop_monitor = NULL;

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

static const struct luaL_Reg lua_monitor_metatable[] = {
  {"__gc", lua_monitor__gc},
  {"__index", lua_monitor__index},
  {"__newindex", lua_monitor__newindex},
  {NULL, NULL}};

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
}
