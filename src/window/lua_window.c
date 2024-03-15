#include "lua_window.h"
#include "utils/string.h"
#include "window/lua_window_list.h"
#include "window/window_properties.h"
#include <lauxlib.h>

static int
lua_catnip_window_method_destroy(lua_State* L)
{
  struct catnip_window* window = lua_catnip_resource_checkmethod(L, "window");
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

  if (streq(key, "x")) {
    lua_pushnumber(L, catnip_window_get_x(window));
  } else if (streq(key, "y")) {
    lua_pushnumber(L, catnip_window_get_y(window));
  } else if (streq(key, "z")) {
    lua_pushnumber(L, catnip_window_get_z(window));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, catnip_window_get_width(window));
  } else if (streq(key, "height")) {
    lua_pushnumber(L, catnip_window_get_height(window));
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, catnip_window_get_visible(window));
  } else if (streq(key, "focused")) {
    lua_pushboolean(L, catnip_window_get_focused(window));
  } else if (streq(key, "fullscreen")) {
    lua_pushboolean(L, catnip_window_get_fullscreen(window));
  } else if (streq(key, "maximized")) {
    lua_pushboolean(L, catnip_window_get_maximized(window));
  } else if (streq(key, "destroy")) {
    lua_pushcfunction(L, lua_catnip_window_method_destroy);
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
    catnip_window_set_x(window, luaL_checknumber(L, 3));
  } else if (streq(key, "y")) {
    catnip_window_set_y(window, luaL_checknumber(L, 3));
  } else if (streq(key, "z")) {
    catnip_window_set_z(window, luaL_checknumber(L, 3));
  } else if (streq(key, "width")) {
    catnip_window_set_width(window, luaL_checknumber(L, 3));
  } else if (streq(key, "height")) {
    catnip_window_set_height(window, luaL_checknumber(L, 3));
  } else if (streq(key, "visible")) {
    catnip_window_set_visible(window, lua_toboolean(L, 3));
  } else if (streq(key, "focused")) {
    catnip_window_set_focused(window, lua_toboolean(L, 3));
  } else if (streq(key, "fullscreen")) {
    catnip_window_set_fullscreen(window, lua_toboolean(L, 3));
  } else if (streq(key, "maximized")) {
    catnip_window_set_maximized(window, lua_toboolean(L, 3));
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
  lua_resource->namespace = "window";
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
