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
catnip_lua_window_close(lua_State* L)
{
  struct catnip_window* window = catnip_lua_resource_checkname(L, 1, "window");
  wlr_xdg_toplevel_send_close(window->wlr.xdg_toplevel);
  return 0;
}

static bool
catnip_lua_window__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_window* window = lua_resource->data;

  if (streq(key, "id")) {
    lua_pushnumber(L, window->id);
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
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static bool
catnip_lua_window__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_window* window = lua_resource->data;

  if (streq(key, "x")) {
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
  } else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct catnip_lua_resource*
catnip_lua_window_create(lua_State* L, struct catnip_window* window)
{
  struct catnip_lua_resource* lua_resource = catnip_lua_resource_create(L);
  window->lua_resource = lua_resource;

  lua_resource->data = window;
  lua_resource->name = "window";
  lua_resource->__index = catnip_lua_window__index;
  lua_resource->__newindex = catnip_lua_window__newindex;

  wl_list_insert(&catnip_lua_windows->windows, &lua_resource->link);

  catnip_lua_window_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
catnip_lua_window_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  catnip_lua_window_publish(L, lua_resource, "destroy", 0);
  catnip_lua_resource_destroy(L, lua_resource);
}

void
catnip_lua_window_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
)
{
  catnip_lua_resource_publish(L, lua_resource, event, nargs);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
  lua_insert(L, -1 - nargs);

  catnip_lua_events_publish(
    L,
    catnip_lua_windows->subscriptions,
    event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
}
