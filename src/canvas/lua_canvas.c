#include "lua_canvas.h"
#include "canvas/canvas.h"
#include "canvas/lua_canvas_path.h"
#include "canvas/lua_canvas_png.h"
#include "canvas/lua_canvas_rectangle.h"
#include "canvas/lua_canvas_svg.h"
#include "canvas/lua_canvas_text.h"
#include "extensions/lua.h"
#include "extensions/string.h"
#include "extensions/wlroots.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_canvas_clear(lua_State* L)
{
  struct catnip_canvas* canvas = luaL_checkudata(L, 1, "catnip.canvas");
  catnip_canvas_clear(canvas);
  return 0;
}

static int
catnip_lua_canvas__index(lua_State* L)
{
  struct catnip_canvas* canvas = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, canvas->wlr.scene_buffer->node.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, canvas->wlr.scene_buffer->node.y);
  } else if (streq(key, "z")) {
    lua_pushnumber(
      L,
      wlr_scene_node_get_zindex(&canvas->wlr.scene_buffer->node)
    );
  } else if (streq(key, "width")) {
    lua_pushnumber(L, canvas->wlr.buffer.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, canvas->wlr.buffer.height);
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, canvas->wlr.scene_buffer->node.enabled);
  } else if (streq(key, "path")) {
    lua_pushcfunction(L, catnip_lua_canvas_path);
  } else if (streq(key, "rectangle")) {
    lua_pushcfunction(L, catnip_lua_canvas_rectangle);
  } else if (streq(key, "text")) {
    lua_pushcfunction(L, catnip_lua_canvas_text);
  } else if (streq(key, "svg")) {
    lua_pushcfunction(L, catnip_lua_canvas_svg);
  } else if (streq(key, "png")) {
    lua_pushcfunction(L, catnip_lua_canvas_png);
  } else if (streq(key, "clear")) {
    lua_pushcfunction(L, catnip_lua_canvas_clear);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static int
catnip_lua_canvas__newindex(lua_State* L)
{
  struct catnip_canvas* canvas = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  }

  if (streq(key, "x")) {
    wlr_scene_node_set_position(
      &canvas->wlr.scene_buffer->node,
      luaL_checknumber(L, 3),
      canvas->wlr.scene_buffer->node.y
    );
  } else if (streq(key, "y")) {
    wlr_scene_node_set_position(
      &canvas->wlr.scene_buffer->node,
      canvas->wlr.scene_buffer->node.x,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "z")) {
    wlr_scene_node_set_zindex(
      &canvas->wlr.scene_buffer->node,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "width")) {
    catnip_canvas_resize(
      canvas,
      luaL_checknumber(L, 3),
      canvas->wlr.buffer.height
    );
  } else if (streq(key, "height")) {
    catnip_canvas_resize(
      canvas,
      canvas->wlr.buffer.width,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "visible")) {
    wlr_scene_node_set_enabled(
      &canvas->wlr.scene_buffer->node,
      lua_toboolean(L, 3)
    );
  }

  return 0;
}

// -----------------------------------------------------------------------------
// __gc
// -----------------------------------------------------------------------------

static int
catnip_lua_canvas__gc(lua_State* L)
{
  struct catnip_canvas* canvas = lua_touserdata(L, 1);
  catnip_canvas_destroy(canvas);
  return 0;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_canvas_mt[] = {
  {"__index", catnip_lua_canvas__index},
  {"__newindex", catnip_lua_canvas__newindex},
  {"__gc", catnip_lua_canvas__gc},
  {NULL, NULL}
};

void
catnip_lua_canvas_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.canvas");
  luaL_setfuncs(L, catnip_lua_canvas_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_canvas(lua_State* L)
{
  int width = 0;
  int height = 0;
  bool has_options_table = lua_type(L, 1) == LUA_TTABLE;

  if (has_options_table) {
    lua_hasnumberfield(L, 1, "width") && (width = lua_popinteger(L));
    lua_hasnumberfield(L, 1, "height") && (height = lua_popinteger(L));
  }

  struct catnip_canvas* canvas =
    lua_newuserdata(L, sizeof(struct catnip_canvas));
  luaL_setmetatable(L, "catnip.canvas");
  catnip_canvas_setup(canvas, width, height);

  if (has_options_table) {
    wlr_scene_node_set_position(
      &canvas->wlr.scene_buffer->node,
      lua_hasnumberfield(L, 1, "x") ? lua_popinteger(L) : 0,
      lua_hasnumberfield(L, 1, "y") ? lua_popinteger(L) : 0
    );

    if (lua_hasnumberfield(L, 1, "z")) {
      wlr_scene_node_set_zindex(
        &canvas->wlr.scene_buffer->node,
        lua_popinteger(L)
      );
    }

    if (lua_hasbooleanfield(L, 1, "visible")) {
      wlr_scene_node_set_enabled(
        &canvas->wlr.scene_buffer->node,
        lua_popboolean(L)
      );
    }
  }

  return 1;
}
