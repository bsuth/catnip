#include "lua_canvas.h"
#include "canvas/canvas.h"
#include "canvas/canvas_methods.h"
#include "canvas/lua_canvas_methods.h"
#include "canvas/lua_canvas_path.h"
#include "canvas/lua_canvas_text.h"
#include "lua_resource.h"
#include "utils/lua.h"
#include "utils/string.h"
#include "utils/wayland.h"
#include <lauxlib.h>

static bool
lua_catnip_canvas__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_canvas* canvas = lua_resource->data;

  if (streq(key, "x")) {
    lua_pushnumber(L, canvas->scene_buffer->node.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, canvas->scene_buffer->node.y);
  } else if (streq(key, "z")) {
    lua_pushnumber(L, wlr_scene_node_get_zindex(&canvas->scene_buffer->node));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, canvas->buffer.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, canvas->buffer.height);
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, canvas->scene_buffer->node.enabled);
  } else if (streq(key, "path")) {
    lua_pushcfunction(L, lua_catnip_canvas_path);
  } else if (streq(key, "text")) {
    lua_pushcfunction(L, lua_catnip_canvas_text);
  } else if (streq(key, "svg")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_svg);
  } else if (streq(key, "png")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_png);
  } else if (streq(key, "clear")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_clear);
  } else if (streq(key, "move")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_move);
  } else if (streq(key, "resize")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_resize);
  } else if (streq(key, "destroy")) {
    lua_pushcfunction(L, lua_catnip_canvas_method_destroy);
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
    wlr_scene_node_set_position(
      &canvas->scene_buffer->node,
      luaL_checknumber(L, 3),
      canvas->scene_buffer->node.y
    );
  } else if (streq(key, "y")) {
    wlr_scene_node_set_position(
      &canvas->scene_buffer->node,
      canvas->scene_buffer->node.x,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "z")) {
    wlr_scene_node_set_zindex(
      &canvas->scene_buffer->node,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "width")) {
    catnip_canvas_resize(canvas, luaL_checknumber(L, 3), canvas->buffer.height);
  } else if (streq(key, "height")) {
    catnip_canvas_resize(canvas, canvas->buffer.width, luaL_checknumber(L, 3));
  } else if (streq(key, "visible")) {
    wlr_scene_node_set_enabled(
      &canvas->scene_buffer->node,
      lua_toboolean(L, 3)
    );
  } else {
    return false;
  }

  return true;
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

  struct catnip_canvas* canvas = catnip_canvas_create(width, height);

  if (has_options_table) {
    wlr_scene_node_set_position(
      &canvas->scene_buffer->node,
      lua_hasnumberfield(L, 1, "x") ? lua_popinteger(L) : 0,
      lua_hasnumberfield(L, 1, "y") ? lua_popinteger(L) : 0
    );

    if (lua_hasnumberfield(L, 1, "z")) {
      wlr_scene_node_set_zindex(&canvas->scene_buffer->node, lua_popinteger(L));
    }

    if (lua_hasbooleanfield(L, 1, "visible")) {
      wlr_scene_node_set_enabled(
        &canvas->scene_buffer->node,
        lua_popboolean(L)
      );
    }
  }

  canvas->lua_resource = lua_catnip_resource_create(L);
  canvas->lua_resource->data = canvas;
  canvas->lua_resource->name = "canvas";
  canvas->lua_resource->__index = lua_catnip_canvas__index;
  canvas->lua_resource->__newindex = lua_catnip_canvas__newindex;

  lua_rawgeti(L, LUA_REGISTRYINDEX, canvas->lua_resource->ref);

  return 1;
}
