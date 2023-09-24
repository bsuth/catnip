// -----------------------------------------------------------------------------
// TODO: document
//
// - destructors
//   - cleanup proxy when lua does gc
//   - remove from catnip.windows when window destroyed, invalidate ptr
//   - separation needed in case user keeps ptr to userdata themselves
// -----------------------------------------------------------------------------

#include "server/desktop/windows.h"
#include "server/lua_api/windows.h"
#include "server/server/seat.h"
#include "server/user_config/user_config.h"
#include "server/utils/wayland.h"
#include <lauxlib.h>
#include <wlr/types/wlr_scene.h>

struct lua_window {
  struct desktop_window* desktop_window;
  struct wl_listener destroy_listener;
  int index;
};

// Keep track of num_windows manually. keeps things simpler when manipulating
// luas catnip.windows
static int num_windows = 0;

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
lua_window__index(lua_State* L)
{
  struct lua_window* lua_window = lua_touserdata(L, 1);
  struct desktop_window* desktop_window = lua_window->desktop_window;

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
lua_window__newindex(lua_State* L)
{
  struct lua_window* lua_window = lua_touserdata(L, 1);
  struct desktop_window* desktop_window = lua_window->desktop_window;

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
lua_window__gc(lua_State* L)
{
  struct lua_window* lua_window = lua_touserdata(L, 1);
  wl_list_remove(&lua_window->destroy_listener.link);
  return 0;
}

// -----------------------------------------------------------------------------
// create_lua_desktop_window
// -----------------------------------------------------------------------------

static void
lua_window_desktop_window_destroy(struct wl_listener* listener, void* data)
{
  struct lua_window* lua_window =
    wl_container_of(listener, lua_window, destroy_listener);

  lua_window->desktop_window = NULL;

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

    struct lua_window* last_lua_window = lua_touserdata(L, -1);
    last_lua_window->index = lua_window->index;

    lua_rawseti(L, -2, lua_window->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, num_windows);
  }

  --num_windows;

  lua_pop(L, 4);
}

void
create_lua_desktop_window(struct desktop_window* desktop_window)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_getfield(L, -1, "windows");

  struct lua_window* lua_window = lua_newuserdata(L, sizeof(struct lua_window));
  luaL_setmetatable(L, "catnip.window");

  ++num_windows;
  lua_window->desktop_window = desktop_window;
  lua_window->index = num_windows;

  wl_setup_listener(
    &lua_window->destroy_listener,
    &desktop_window->xdg_surface->events.destroy,
    lua_window_desktop_window_destroy
  );

  lua_rawseti(L, -2, lua_window->index);

  lua_pop(L, 4);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

static const struct luaL_Reg lua_window_metatable[] = {
  {"__gc", lua_window__gc},
  {"__index", lua_window__index},
  {"__newindex", lua_window__newindex},
  {NULL, NULL}};

void
init_lua_windows(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");
  lua_newtable(L);
  lua_setfield(L, -2, "windows");
  lua_pop(L, 3);

  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, lua_window_metatable, 0);
  lua_pop(L, 1);
}
