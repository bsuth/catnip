#include "window.h"
#include "desktop/window.h"
#include "server/seat.h"
#include "user_config/user_config.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>
#include <string.h>
#include <wlr/types/wlr_scene.h>

struct api_window {
  struct desktop_window* desktop_window;
  struct wl_listener destroy_listener;
  int index;
};

// Keep track of num_windows manually. keeps things simpler when manipulating
// Lua's catnip.windows
static int num_windows = 0;

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
api_window__index(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);
  struct desktop_window* desktop_window = api_window->desktop_window;

  if (desktop_window == NULL || lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "lx")) {
    lua_pushnumber(L, desktop_window_get_lx(desktop_window));
  } else if (g_str_equal(field, "ly")) {
    lua_pushnumber(L, desktop_window_get_ly(desktop_window));
  } else if (g_str_equal(field, "gx")) {
    lua_pushnumber(L, desktop_window_get_gx(desktop_window));
  } else if (g_str_equal(field, "gy")) {
    lua_pushnumber(L, desktop_window_get_gy(desktop_window));
  } else if (g_str_equal(field, "width")) {
    lua_pushnumber(L, desktop_window_get_width(desktop_window));
  } else if (g_str_equal(field, "height")) {
    lua_pushnumber(L, desktop_window_get_height(desktop_window));
  } else if (g_str_equal(field, "focused")) {
    lua_pushboolean(L, desktop_window_get_focused(desktop_window));
  } else if (g_str_equal(field, "maximized")) {
    lua_pushboolean(L, desktop_window_get_maximized(desktop_window));
  } else if (g_str_equal(field, "fullscreen")) {
    lua_pushboolean(L, desktop_window_get_fullscreen(desktop_window));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_window__newindex(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);
  struct desktop_window* desktop_window = api_window->desktop_window;

  if (desktop_window == NULL || lua_type(L, 2) != LUA_TSTRING) {
    return 0; // TODO: error?
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "lx")) {
    desktop_window_set_lx(desktop_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "ly")) {
    desktop_window_set_ly(desktop_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "gx")) {
    desktop_window_set_gx(desktop_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "gy")) {
    desktop_window_set_gy(desktop_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "width")) {
    desktop_window_set_width(desktop_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "height")) {
    desktop_window_set_height(desktop_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "focused")) {
    desktop_window_set_focused(desktop_window, lua_toboolean(L, 3));
  } else if (g_str_equal(field, "maximized")) {
    desktop_window_set_maximized(desktop_window, lua_toboolean(L, 3));
  } else if (g_str_equal(field, "fullscreen")) {
    desktop_window_set_fullscreen(desktop_window, lua_toboolean(L, 3));
  } else {
    // TODO: error?
  }

  return 0;
}

static int
api_window__gc(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);
  wl_list_remove(&api_window->destroy_listener.link);
  return 0;
}

// -----------------------------------------------------------------------------
// create_api_desktop_window
// -----------------------------------------------------------------------------

static void
api_window_desktop_window_destroy(struct wl_listener* listener, void* data)
{
  struct api_window* api_window =
    wl_container_of(listener, api_window, destroy_listener);

  api_window->desktop_window = NULL;

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_getfield(L, -1, "windows");

  if (num_windows == 1) {
    lua_pushnil(L);
    lua_rawseti(L, -2, 1);
  } else {
    // Quick delete by moving last element
    lua_rawgeti(L, -1, num_windows);

    struct api_window* last_api_window = lua_touserdata(L, -1);
    last_api_window->index = api_window->index;

    lua_rawseti(L, -2, api_window->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, num_windows);
  }

  --num_windows;

  lua_pop(L, 4);
}

void
create_api_desktop_window(struct desktop_window* desktop_window)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_getfield(L, -1, "windows");

  struct api_window* api_window = lua_newuserdata(L, sizeof(struct api_window));
  luaL_setmetatable(L, "catnip.window");

  ++num_windows;
  api_window->desktop_window = desktop_window;
  api_window->index = num_windows;

  wl_setup_listener(
    &api_window->destroy_listener,
    &desktop_window->xdg_surface->events.destroy,
    api_window_desktop_window_destroy
  );

  lua_rawseti(L, -2, api_window->index);

  lua_pop(L, 4);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

static const struct luaL_Reg api_window_metatable[] = {
  {"__gc", api_window__gc},
  {"__index", api_window__index},
  {"__newindex", api_window__newindex},
  {NULL, NULL}};

void
init_api_windows(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_newtable(L);
  lua_setfield(L, -2, "windows");
  lua_pop(L, 3);

  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, api_window_metatable, 0);
  lua_pop(L, 1);
}
