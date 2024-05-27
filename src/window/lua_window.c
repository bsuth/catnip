#include "lua_window.h"
#include "utils/string.h"
#include "utils/wayland.h"
#include "window/lua_window_list.h"
#include <lauxlib.h>
#include <wlr/types/wlr_scene.h>

static int
lua_catnip_window_close(lua_State* L)
{
  struct catnip_window* window = lua_catnip_resource_checkname(L, 1, "window");
  wlr_xdg_toplevel_send_close(window->xdg_toplevel);
  return 0;
}

static bool
lua_catnip_window__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_window* window = lua_resource->data;

  if (streq(key, "id")) {
    lua_pushnumber(L, window->id);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, window->scene_tree->node.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, window->scene_tree->node.y);
  } else if (streq(key, "z")) {
    lua_pushnumber(L, wlr_scene_node_get_zindex(&window->scene_tree->node));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, window->xdg_toplevel->current.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, window->xdg_toplevel->current.height);
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, window->scene_tree->node.enabled);
  } else if (streq(key, "title")) {
    lua_pushstring(L, window->xdg_toplevel->title);
  } else if (streq(key, "destroy")) {
    lua_pushcfunction(L, lua_catnip_window_close);
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_window__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_window* window = lua_resource->data;

  if (streq(key, "x")) {
    wlr_scene_node_set_position(
      &window->scene_tree->node,
      luaL_checkinteger(L, 3),
      window->scene_tree->node.y
    );
  } else if (streq(key, "y")) {
    wlr_scene_node_set_position(
      &window->scene_tree->node,
      window->scene_tree->node.x,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "z")) {
    wlr_scene_node_set_zindex(
      &window->scene_tree->node,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "width")) {
    wlr_xdg_toplevel_set_size(
      window->xdg_toplevel,
      luaL_checkinteger(L, 3),
      window->xdg_toplevel->scheduled.height
    );
  } else if (streq(key, "height")) {
    wlr_xdg_toplevel_set_size(
      window->xdg_toplevel,
      window->xdg_toplevel->scheduled.width,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "visible")) {
    wlr_scene_node_set_enabled(&window->scene_tree->node, lua_toboolean(L, 3));
  } else {
    return false;
  }

  return true;
}

struct catnip_lua_resource*
lua_catnip_window_create(lua_State* L, struct catnip_window* window)
{
  struct catnip_lua_resource* lua_resource = lua_catnip_resource_create(L);
  window->lua_resource = lua_resource;

  lua_resource->data = window;
  lua_resource->name = "window";
  lua_resource->__index = lua_catnip_window__index;
  lua_resource->__newindex = lua_catnip_window__newindex;

  wl_list_insert(&lua_catnip_window_list->head, &lua_resource->link);

  lua_catnip_resource_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
lua_catnip_window_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  lua_catnip_resource_publish(L, lua_resource, "destroy", 0);
  lua_catnip_resource_destroy(L, lua_resource);
}
