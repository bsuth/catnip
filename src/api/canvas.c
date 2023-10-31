#include "canvas.h"
#include "api/api.h"
#include "canvas/canvas.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <glib.h>
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static int
parse_percent_str(const char* str)
{
  const int len = strlen(str);

  if (str[len - 1] != '%') {
    return -1;
  }

  int percent = 0;

  for (int i = 0; i < len - 1; ++i) {
    if (str[i] < '0' || str[i] > '9') {
      return -1;
    } else {
      percent = 10 * percent + (str[i] - '0');
    }
  }

  return percent;
}

// -----------------------------------------------------------------------------
// Drawing
// -----------------------------------------------------------------------------

static int
canvas_clear(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *api_canvas;

  cairo_save(canvas->cr);
  cairo_set_operator(canvas->cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);

  return 0;
}

static int
canvas_rectangle(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *api_canvas;

  int x = 0;
  int y = 0;
  int width = 100;
  int height = 100;
  int bg = 0xFF0000;

  double r = ((double) ((bg & 0xFF0000) >> 16)) / 0xFF;
  double g = ((double) ((bg & 0x00FF00) >> 8)) / 0xFF;
  double b = ((double) (bg & 0x0000FF)) / 0xFF;

  cairo_save(canvas->cr);
  cairo_set_source_rgb(canvas->cr, r, g, b);
  cairo_rectangle(canvas->cr, x, y, width, height);
  cairo_fill(canvas->cr);
  cairo_restore(canvas->cr);

  return 0;
}

// -----------------------------------------------------------------------------
// Metatable: catnip.canvas
// -----------------------------------------------------------------------------

static int
api_canvas__index(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *api_canvas;

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, canvas_get_x(canvas));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, canvas_get_y(canvas));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, canvas_get_width(canvas));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, canvas_get_height(canvas));
  } else if (g_str_equal(key, "visible")) {
    lua_pushboolean(L, canvas_get_visible(canvas));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_canvas__newindex(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *api_canvas;

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    log_warning("invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    canvas_set_x(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    canvas_set_y(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    canvas_set_width(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    canvas_set_height(canvas, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "visible")) {
    canvas_set_visible(canvas, lua_toboolean(L, 3));
  } else {
    log_warning("invalid key: %s", key);
  }

  return 0;
}

static int
api_canvas__gc(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  canvas_destroy(*api_canvas);
  return 0;
}

static const struct luaL_Reg api_canvas_metatable[] = {
  {"__index", api_canvas__index},
  {"__newindex", api_canvas__newindex},
  {"__gc", api_canvas__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// API Canvas
// -----------------------------------------------------------------------------

static int
api_canvas_create(lua_State* L)
{
  struct catnip_canvas** api_canvas =
    lua_newuserdata(L, sizeof(struct catnip_canvas*));
  luaL_setmetatable(L, "catnip.canvas");

  int width = 0;
  int height = 0;
  bool has_options_table = lua_type(L, 1) == LUA_TTABLE;

  if (has_options_table) {
    if (lua_hasfield(L, 1, "width")) {
      width = lua_popnumber(L);
    }

    if (lua_hasfield(L, 1, "height")) {
      height = lua_popnumber(L);
    }
  }

  struct catnip_canvas* canvas = canvas_create(width, height);
  *api_canvas = canvas;

  if (has_options_table) {
    if (lua_hasfield(L, 1, "visible")) {
      canvas_set_visible(canvas, lua_popboolean(L));
    }
  }

  return 1;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
api_canvas_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.canvas");
  luaL_setfuncs(L, api_canvas_metatable, 0);
  lua_pop(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);
  lua_pushcfunction(L, api_canvas_create);
  lua_setfield(L, -2, "canvas");
  lua_pop(L, 1);
}
