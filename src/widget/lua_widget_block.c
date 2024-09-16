#include "lua_widget_block.h"
#include "extensions/lua.h"
#include "extensions/string.h"
#include "widget/lua_widget_base.h"
#include "widget/widget_block.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_block__index(lua_State* L, struct catnip_widget_base* base)
{
  struct catnip_widget_block* block = base->data;
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, 0); // TODO
  } else if (streq(key, "y")) {
    lua_pushnumber(L, 0); // TODO
  } else if (streq(key, "width")) {
    lua_pushnumber(L, 0); // TODO
  } else if (streq(key, "height")) {
    lua_pushnumber(L, 0); // TODO
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_block__newindex(lua_State* L, struct catnip_widget_base* base)
{
  struct catnip_widget_block* block = base->data;
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return;
  }

  if (streq(key, "x")) {
    // TODO
  } else if (streq(key, "y")) {
    // TODO
  } else if (streq(key, "width")) {
    // TODO
  } else if (streq(key, "height")) {
    // TODO
  }
}

// -----------------------------------------------------------------------------
// __gc
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_block__gc(lua_State* L, struct catnip_widget_base* base)
{
  struct catnip_widget_block* block = base->data;
  catnip_widget_block_destroy(block);
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static struct catnip_lua_widget_base_mt catnip_lua_widget_block_mt = {
  .__index = catnip_lua_widget_block__index,
  .__newindex = catnip_lua_widget_block__newindex,
  .__gc = catnip_lua_widget_block__gc,
};

int
catnip_lua_widget_block(lua_State* L)
{
  struct catnip_widget_block* block = catnip_widget_block_create();

  struct catnip_widget_base* base = catnip_lua_widget_base(L);
  base->parent = NULL;
  base->data = block;
  base->type = CATNIP_WIDGET_BLOCK;
  base->mt = &catnip_lua_widget_block_mt;

  if (lua_type(L, 1) == LUA_TTABLE) {
    block->styles.x = lua_hasnumberfield(L, 1, "x") ? lua_popnumber(L) : -1;
    block->styles.y = lua_hasnumberfield(L, 1, "y") ? lua_popnumber(L) : -1;

    block->styles.width =
      lua_hasnumberfield(L, 1, "width") ? lua_popnumber(L) : -1;
    block->styles.height =
      lua_hasnumberfield(L, 1, "height") ? lua_popnumber(L) : -1;

    block->styles.padding =
      lua_hasnumberfield(L, 1, "padding") ? lua_popnumber(L) : 0;
    block->styles.padding_top =
      lua_hasnumberfield(L, 1, "padding_top") ? lua_popnumber(L) : -1;
    block->styles.padding_left =
      lua_hasnumberfield(L, 1, "padding_left") ? lua_popnumber(L) : -1;
    block->styles.padding_bottom =
      lua_hasnumberfield(L, 1, "padding_bottom") ? lua_popnumber(L) : -1;
    block->styles.padding_right =
      lua_hasnumberfield(L, 1, "padding_right") ? lua_popnumber(L) : -1;

    block->styles.radius =
      lua_hasnumberfield(L, 1, "radius") ? lua_popnumber(L) : 0;
    block->styles.radius_top_left =
      lua_hasnumberfield(L, 1, "radius_top_left") ? lua_popnumber(L) : -1;
    block->styles.radius_top_right =
      lua_hasnumberfield(L, 1, "radius_top_right") ? lua_popnumber(L) : -1;
    block->styles.radius_bottom_left =
      lua_hasnumberfield(L, 1, "radius_bottom_left") ? lua_popnumber(L) : -1;
    block->styles.radius_bottom_right =
      lua_hasnumberfield(L, 1, "radius_bottom_right") ? lua_popnumber(L) : -1;

    block->styles.bg_color =
      lua_hasnumberfield(L, 1, "bg_color") ? lua_popnumber(L) : -1;
    block->styles.bg_opacity =
      lua_hasnumberfield(L, 1, "bg_opacity") ? lua_popnumber(L) : 1;

    block->styles.border_color =
      lua_hasnumberfield(L, 1, "border_color") ? lua_popnumber(L) : -1;
    block->styles.border_opacity =
      lua_hasnumberfield(L, 1, "border_opacity") ? lua_popnumber(L) : 1;
    block->styles.border_width =
      lua_hasnumberfield(L, 1, "border_width") ? lua_popnumber(L) : -1;
  }

  return 1;
}
