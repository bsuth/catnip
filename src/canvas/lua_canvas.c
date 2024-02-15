#include "lua_canvas.h"
#include "canvas/canvas.h"
#include "canvas/canvas_properties.h"
#include "canvas/lua_canvas_methods.h"
#include "canvas/lua_canvas_png.h"
#include "canvas/lua_canvas_rectangle.h"
#include "canvas/lua_canvas_svg.h"
#include "canvas/lua_canvas_text.h"
#include "utils/lua.h"
#include <glib.h>
#include <lauxlib.h>

static int
lua_catnip_canvas__index(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *lua_canvas;

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, catnip_canvas_get_x(canvas));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, catnip_canvas_get_y(canvas));
  } else if (g_str_equal(key, "z")) {
    lua_pushnumber(L, catnip_canvas_get_z(canvas));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, catnip_canvas_get_width(canvas));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, catnip_canvas_get_height(canvas));
  } else if (g_str_equal(key, "visible")) {
    lua_pushboolean(L, catnip_canvas_get_visible(canvas));
  } else if (g_str_equal(key, "clear")) {
    lua_pushcfunction(L, lua_catnip_canvas_clear);
  } else if (g_str_equal(key, "rectangle")) {
    lua_pushcfunction(L, lua_catnip_canvas_rectangle);
  } else if (g_str_equal(key, "text")) {
    lua_pushcfunction(L, lua_catnip_canvas_text);
  } else if (g_str_equal(key, "svg")) {
    lua_pushcfunction(L, lua_catnip_canvas_svg);
  } else if (g_str_equal(key, "png")) {
    lua_pushcfunction(L, lua_catnip_canvas_png);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_canvas__newindex(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *lua_canvas;

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    catnip_canvas_set_x(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    catnip_canvas_set_y(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "z")) {
    catnip_canvas_set_z(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    catnip_canvas_set_width(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    catnip_canvas_set_height(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "visible")) {
    catnip_canvas_set_visible(canvas, lua_toboolean(L, 3));
  } else {
    lua_log_error(L, "unknown userdata field (%s)", key);
  }

  return 0;
}

static int
lua_catnip_canvas__gc(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  catnip_canvas_destroy(*lua_canvas);
  return 0;
}

static const struct luaL_Reg lua_catnip_canvas_metatable[] = {
  {"__index", lua_catnip_canvas__index},
  {"__newindex", lua_catnip_canvas__newindex},
  {"__gc", lua_catnip_canvas__gc},
  {NULL, NULL}
};

void
lua_catnip_canvas_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.canvas");
  luaL_setfuncs(L, lua_catnip_canvas_metatable, 0);
  lua_pop(L, 1);
}

int
lua_catnip_canvas_create(lua_State* L)
{
  struct catnip_canvas** api_canvas =
    lua_newuserdata(L, sizeof(struct catnip_canvas*));
  luaL_setmetatable(L, "catnip.canvas");

  int width = 0;
  int height = 0;
  bool has_options_table = lua_type(L, 1) == LUA_TTABLE;

  if (has_options_table) {
    lua_hasnumberfield(L, 1, "width") && (width = lua_popinteger(L));
    lua_hasnumberfield(L, 1, "height") && (height = lua_popinteger(L));
  }

  struct catnip_canvas* canvas = catnip_canvas_create(width, height);
  *api_canvas = canvas;

  if (has_options_table) {
    if (lua_hasbooleanfield(L, 1, "visible")) {
      catnip_canvas_set_visible(canvas, lua_popboolean(L));
    }
  }

  return 1;
}
