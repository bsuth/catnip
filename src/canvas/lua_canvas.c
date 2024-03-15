#include "lua_canvas.h"
#include "canvas/canvas.h"
#include "canvas/canvas_methods.h"
#include "canvas/canvas_properties.h"
#include "canvas/lua_canvas_path.h"
#include "canvas/lua_canvas_png.h"
#include "canvas/lua_canvas_svg.h"
#include "canvas/lua_canvas_text.h"
#include "lua_resource.h"
#include "utils/lua.h"
#include "utils/string.h"
#include <lauxlib.h>

static int
lua_catnip_canvas_method_clear(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkmethod(L, "canvas");
  catnip_canvas_clear(canvas);
  return 0;
}

static bool
lua_catnip_canvas__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_canvas* canvas = lua_resource->data;

  if (streq(key, "x")) {
    lua_pushnumber(L, catnip_canvas_get_x(canvas));
  } else if (streq(key, "y")) {
    lua_pushnumber(L, catnip_canvas_get_y(canvas));
  } else if (streq(key, "z")) {
    lua_pushnumber(L, catnip_canvas_get_z(canvas));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, catnip_canvas_get_width(canvas));
  } else if (streq(key, "height")) {
    lua_pushnumber(L, catnip_canvas_get_height(canvas));
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, catnip_canvas_get_visible(canvas));
  } else if (streq(key, "clear")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_clear);
  } else if (streq(key, "path")) {
    lua_pushcfunction(L, lua_catnip_canvas_path);
  } else if (streq(key, "text")) {
    lua_pushcfunction(L, lua_catnip_canvas_text);
  } else if (streq(key, "svg")) {
    lua_pushcfunction(L, lua_catnip_canvas_svg);
  } else if (streq(key, "png")) {
    lua_pushcfunction(L, lua_catnip_canvas_png);
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_canvas__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_canvas* canvas = lua_resource->data;

  if (streq(key, "x")) {
    catnip_canvas_set_x(canvas, luaL_checknumber(L, 3));
  } else if (streq(key, "y")) {
    catnip_canvas_set_y(canvas, luaL_checknumber(L, 3));
  } else if (streq(key, "z")) {
    catnip_canvas_set_z(canvas, luaL_checknumber(L, 3));
  } else if (streq(key, "width")) {
    catnip_canvas_set_width(canvas, luaL_checknumber(L, 3));
  } else if (streq(key, "height")) {
    catnip_canvas_set_height(canvas, luaL_checknumber(L, 3));
  } else if (streq(key, "visible")) {
    catnip_canvas_set_visible(canvas, lua_toboolean(L, 3));
  } else {
    return false;
  }

  return true;
}

static void
lua_catnip_canvas__gc(lua_State* L, struct catnip_lua_resource* lua_resource)
{
  struct catnip_canvas* canvas = lua_resource->data;
  catnip_canvas_destroy(canvas);
}

int
lua_catnip_canvas(lua_State* L)
{
  int width = 0;
  int height = 0;
  bool has_options_table = lua_type(L, 1) == LUA_TTABLE;

  if (has_options_table) {
    lua_hasnumberfield(L, 1, "width") && (width = lua_popinteger(L));
    lua_hasnumberfield(L, 1, "height") && (height = lua_popinteger(L));
  }

  struct catnip_lua_resource* lua_resource = lua_catnip_resource_create(L);
  struct catnip_canvas* canvas = catnip_canvas_create(width, height);

  lua_resource->data = canvas;
  lua_resource->namespace = "canvas";
  lua_resource->__index = lua_catnip_canvas__index;
  lua_resource->__newindex = lua_catnip_canvas__newindex;
  lua_resource->__gc = lua_catnip_canvas__gc;

  if (has_options_table) {
    if (lua_hasbooleanfield(L, 1, "visible")) {
      catnip_canvas_set_visible(canvas, lua_popboolean(L));
    }
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);

  return 1;
}
