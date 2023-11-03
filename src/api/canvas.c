#include "canvas.h"
#include "api/api.h"
#include "canvas/canvas.h"
#include "canvas/shapes.h"
#include "canvas/text.h"
#include "pango/pango-layout.h"
#include "utils/cairo.h"
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
api_canvas_clear(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  canvas_clear(*api_canvas);
  return 0;
}

static int
api_canvas_rectangle(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *api_canvas;

  luaL_checktype(L, 6, LUA_TTABLE);

  struct canvas_rectangle rectangle = {
    .x = luaL_checknumber(L, 2),
    .y = luaL_checknumber(L, 3),
    .width = luaL_checknumber(L, 4),
    .height = luaL_checknumber(L, 5),
    .bg = lua_hasfield(L, 6, "bg") ? lua_popinteger(L) : -1,
    .border = lua_hasfield(L, 6, "border") ? lua_popinteger(L) : -1,
    .opacity = lua_hasfield(L, 6, "opacity") ? lua_popnumber(L) : 1,
    .radius = lua_hasfield(L, 6, "radius") ? lua_popnumber(L) : 0,
  };

  rectangle.border_width = lua_hasfield(L, 6, "border_width")
                             ? lua_popinteger(L)
                             : (rectangle.border != -1);

  canvas_rectangle(canvas, &rectangle);

  return 0;
}

static int
api_canvas_text(lua_State* L)
{
  struct catnip_canvas** api_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *api_canvas;

  struct canvas_text text = {
    .markup = luaL_checkstring(L, 2),
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
    .alignment = PANGO_ALIGN_LEFT,
    .ellipsize = PANGO_ELLIPSIZE_NONE,
    .wrap = PANGO_WRAP_WORD_CHAR,
  };

  if (lua_type(L, 3) == LUA_TTABLE) {
    lua_hasfield(L, 3, "x") && (text.x = lua_popinteger(L));
    lua_hasfield(L, 3, "y") && (text.y = lua_popinteger(L));
    lua_hasfield(L, 3, "width")
      && (text.width = PANGO_SCALE * lua_popinteger(L));

    if (lua_hasfield(L, 3, "height")) {
      text.height = lua_popinteger(L);
      (text.height > 0) && (text.height *= PANGO_SCALE);
    }

    if (lua_hasfield(L, 3, "align")) {
      const char* alignment = lua_popstring(L);

      if (g_str_equal(alignment, "left")) {
        text.alignment = PANGO_ALIGN_LEFT;
      } else if (g_str_equal(alignment, "center")) {
        text.alignment = PANGO_ALIGN_CENTER;
      } else if (g_str_equal(alignment, "right")) {
        text.alignment = PANGO_ALIGN_RIGHT;
      } else {
        log_warning("invalid text alignment: %s", alignment);
      }
    }

    if (lua_hasfield(L, 3, "ellipsis")) {
      int ellipsis_type = lua_type(L, -1);

      if (ellipsis_type == LUA_TBOOLEAN) {
        text.ellipsize =
          lua_toboolean(L, -1) ? PANGO_ELLIPSIZE_END : PANGO_ELLIPSIZE_NONE;
      } else if (ellipsis_type == LUA_TSTRING) {
        const char* ellipsis = lua_tostring(L, -1);

        if (g_str_equal(ellipsis, "start")) {
          text.ellipsize = PANGO_ELLIPSIZE_START;
        } else if (g_str_equal(ellipsis, "middle")) {
          text.ellipsize = PANGO_ELLIPSIZE_MIDDLE;
        } else if (g_str_equal(ellipsis, "end")) {
          text.ellipsize = PANGO_ELLIPSIZE_END;
        } else {
          log_warning("invalid ellipsis: %s", ellipsis);
        }
      } else {
        log_warning(
          "invalid ellipsis type: %s",
          lua_typename(L, ellipsis_type)
        );
      }

      lua_pop(L, 1);
    }

    if (lua_hasfield(L, 3, "wrap")) {
      const char* wrap = lua_popstring(L);

      if (g_str_equal(wrap, "char")) {
        text.wrap = PANGO_WRAP_CHAR;
      } else if (g_str_equal(wrap, "word")) {
        text.wrap = PANGO_WRAP_WORD;
      } else if (g_str_equal(wrap, "auto")) {
        text.wrap = PANGO_WRAP_WORD_CHAR;
      } else {
        log_warning("invalid wrap: %s", wrap);
      }
    }
  }

  canvas_text(canvas, &text);

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
  } else if (g_str_equal(key, "clear")) {
    lua_pushcfunction(L, api_canvas_clear);
  } else if (g_str_equal(key, "rectangle")) {
    lua_pushcfunction(L, api_canvas_rectangle);
  } else if (g_str_equal(key, "text")) {
    lua_pushcfunction(L, api_canvas_text);
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
    width = lua_hasfield(L, 1, "width") ? lua_popnumber(L) : width;
    height = lua_hasfield(L, 1, "height") ? lua_popnumber(L) : height;
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
