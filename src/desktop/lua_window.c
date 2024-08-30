#include "lua_window.h"
#include "desktop/lua_windows.h"
#include "extensions/string.h"
#include "extensions/wlroots.h"
#include "lua_events.h"
#include <lauxlib.h>
#include <wlr/types/wlr_scene.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_window_on(lua_State* L)
{
  struct catnip_lua_window* lua_window = luaL_checkudata(L, 1, "catnip.window");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, lua_window->subscriptions, event);

  return 1;
}

static int
catnip_lua_window_emit(lua_State* L)
{
  struct catnip_lua_window* lua_window = luaL_checkudata(L, 1, "catnip.window");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    lua_window->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

static int
catnip_lua_window_close(lua_State* L)
{
  struct catnip_lua_window* lua_window = luaL_checkudata(L, 1, "catnip.window");
  wlr_xdg_toplevel_send_close(lua_window->window->wlr.xdg_toplevel);
  return 0;
}

static int
catnip_lua_window__index(lua_State* L)
{
  struct catnip_lua_window* lua_window = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  struct catnip_window* window = lua_window->window;

  if (window == NULL) {
    lua_log_error(L, "attempt to index outdated window");
    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "id")) {
    lua_pushnumber(L, window->id);
  } else if (streq(key, "on")) {
    lua_pushcfunction(L, catnip_lua_window_on);
  } else if (streq(key, "emit")) {
    lua_pushcfunction(L, catnip_lua_window_emit);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, window->wlr.scene_tree->node.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, window->wlr.scene_tree->node.y);
  } else if (streq(key, "z")) {
    lua_pushnumber(L, wlr_scene_node_get_zindex(&window->wlr.scene_tree->node));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, window->wlr.xdg_toplevel->current.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, window->wlr.xdg_toplevel->current.height);
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, window->wlr.scene_tree->node.enabled);
  } else if (streq(key, "title")) {
    lua_pushstring(L, window->wlr.xdg_toplevel->title);
  } else if (streq(key, "destroy")) {
    lua_pushcfunction(L, catnip_lua_window_close);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static int
catnip_lua_window__newindex(lua_State* L)
{
  struct catnip_lua_window* lua_window = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  struct catnip_window* window = lua_window->window;

  if (key == NULL) {
    return 0;
  }

  if (window == NULL) {
    lua_log_error(L, "attempt to index outdated window");
  } else if (streq(key, "x")) {
    wlr_scene_node_set_position(
      &window->wlr.scene_tree->node,
      luaL_checkinteger(L, 3),
      window->wlr.scene_tree->node.y
    );
  } else if (streq(key, "y")) {
    wlr_scene_node_set_position(
      &window->wlr.scene_tree->node,
      window->wlr.scene_tree->node.x,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "z")) {
    wlr_scene_node_set_zindex(
      &window->wlr.scene_tree->node,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "width")) {
    wlr_xdg_toplevel_set_size(
      window->wlr.xdg_toplevel,
      luaL_checkinteger(L, 3),
      window->wlr.xdg_toplevel->scheduled.height
    );
  } else if (streq(key, "height")) {
    wlr_xdg_toplevel_set_size(
      window->wlr.xdg_toplevel,
      window->wlr.xdg_toplevel->scheduled.width,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "visible")) {
    wlr_scene_node_set_enabled(
      &window->wlr.scene_tree->node,
      lua_toboolean(L, 3)
    );
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_window_mt[] = {
  {"__index", catnip_lua_window__index},
  {"__newindex", catnip_lua_window__newindex},
  {NULL, NULL}
};

void
catnip_lua_window_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, catnip_lua_window_mt, 0);
  lua_pop(L, 1);
}

void
catnip_lua_window_create(lua_State* L, struct catnip_window* window)
{
  struct catnip_lua_window* lua_window =
    lua_newuserdata(L, sizeof(struct catnip_lua_window));
  luaL_setmetatable(L, "catnip.window");

  lua_window->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_window->window = window;

  lua_newtable(L);
  lua_window->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_list_insert(&catnip_lua_windows->windows, &lua_window->link);

  // Assign `window->lua_window` here, since the Lua window may be created much
  // later than the actual window (ex. on config reload).
  window->lua_window = lua_window;

  catnip_lua_window_publish(L, lua_window, "create", 0);
}

void
catnip_lua_window_destroy(lua_State* L, struct catnip_lua_window* lua_window)
{
  catnip_lua_window_publish(L, lua_window, "destroy", 0);

  // Explicitly set `NULL`, just in case the user is keeping a reference.
  lua_window->window = NULL;

  luaL_unref(L, LUA_REGISTRYINDEX, lua_window->subscriptions);
  luaL_unref(L, LUA_REGISTRYINDEX, lua_window->ref);
  wl_list_remove(&lua_window->link);
}

void
catnip_lua_window_publish(
  lua_State* L,
  struct catnip_lua_window* lua_window,
  const char* event,
  int nargs
)
{
  catnip_lua_events_publish(L, lua_window->subscriptions, event, nargs);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_window->ref);
  lua_insert(L, -1 - nargs);

  catnip_lua_events_publish(
    L,
    catnip_lua_windows->subscriptions,
    event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
}
