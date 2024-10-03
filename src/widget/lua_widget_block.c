#include "lua_widget_block.h"
#include "extensions/cairo.h"
#include "extensions/lua.h"
#include "extensions/string.h"
#include "widget/lua_widget_base.h"
#include <lauxlib.h>

static void
catnip_lua_widget_block_link(
  struct catnip_lua_widget_block* block,
  struct catnip_lua_widget_base* child
)
{
  child->root = block->base.root;
  child->parent = &block->base;
}

static void
catnip_lua_widget_block_unlink(
  struct catnip_lua_widget_block* block,
  struct catnip_lua_widget_base* child
)
{
  child->root = NULL;
  child->parent = NULL;
}

// -----------------------------------------------------------------------------
// Lua Methods
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_block_lua_insert(lua_State* L)
{
  struct catnip_lua_widget_block* block =
    luaL_checkudata(L, 1, "catnip.widget.block");

  lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
  int children_len = lua_objlen(L, -1);

  int position = -1;
  struct catnip_lua_widget_base* child = NULL;

  if (lua_type(L, 2) == LUA_TNUMBER) {
    position = lua_tointeger(L, 2);
    child = lua_touserdata(L, 3); // TODO: check type
    lua_pushvalue(L, 3);
  } else {
    position = children_len + 1;
    child = lua_touserdata(L, 2); // TODO: check type
    lua_pushvalue(L, 2);
  }

  // TODO: position bounds

  for (int i = children_len + 1; i > position; --i) {
    lua_rawgeti(L, -2, i - 1);
    lua_rawseti(L, -3, i);
  }

  lua_rawseti(L, -2, position);
  lua_pop(L, 1);

  catnip_lua_widget_block_link(block, child);
  catnip_lua_widget_base_request_layout(&block->base);

  return 0;
}

static int
catnip_lua_widget_block_lua_remove(lua_State* L)
{
  struct catnip_lua_widget_block* block =
    luaL_checkudata(L, 1, "catnip.widget.block");

  lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
  int children_len = lua_objlen(L, -1);

  int position =
    lua_type(L, 2) == LUA_TNUMBER ? lua_tonumber(L, 2) : children_len;

  // TODO: position bounds

  lua_rawgeti(L, -1, position);
  lua_insert(L, -2); // keep for return
  if (lua_type(L, -2) == LUA_TUSERDATA) {
    catnip_lua_widget_block_unlink(block, lua_touserdata(L, -2));
  }

  for (int i = position; i < children_len; ++i) {
    lua_rawgeti(L, -1, i + 1);
    lua_rawseti(L, -2, i);
  }

  lua_pushnil(L);
  lua_rawseti(L, -2, children_len);
  lua_pop(L, 1);

  catnip_lua_widget_base_request_layout(&block->base);

  return 1;
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_block__index(lua_State* L)
{
  struct catnip_lua_widget_block* block = lua_touserdata(L, 1);

  if (lua_type(L, 2) == LUA_TNUMBER) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
    lua_rawgeti(L, -1, lua_tonumber(L, 2));
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, 0); // TODO (need to handle units)
  } else if (streq(key, "y")) {
    lua_pushnumber(L, 0); // TODO (need to handle units)
  } else if (streq(key, "width")) {
    lua_pushnumber(L, 0); // TODO (need to handle units)
  } else if (streq(key, "height")) {
    lua_pushnumber(L, 0); // TODO (need to handle units)
  } else if (streq(key, "padding")) {
    lua_pushnumber(L, block->styles.padding);
  } else if (streq(key, "padding_top")) {
    lua_pushnumber(L, block->styles.padding_top);
  } else if (streq(key, "padding_left")) {
    lua_pushnumber(L, block->styles.padding_left);
  } else if (streq(key, "padding_bottom")) {
    lua_pushnumber(L, block->styles.padding_bottom);
  } else if (streq(key, "padding_right")) {
    lua_pushnumber(L, block->styles.padding_right);
  } else if (streq(key, "radius")) {
    lua_pushnumber(L, block->styles.radius);
  } else if (streq(key, "radius_top_left")) {
    lua_pushnumber(L, block->styles.radius_top_left);
  } else if (streq(key, "radius_top_right")) {
    lua_pushnumber(L, block->styles.radius_top_right);
  } else if (streq(key, "radius_bottom_left")) {
    lua_pushnumber(L, block->styles.radius_bottom_left);
  } else if (streq(key, "radius_bottom_right")) {
    lua_pushnumber(L, block->styles.radius_bottom_right);
  } else if (streq(key, "bg_color")) {
    lua_pushnumber(L, block->styles.bg_color);
  } else if (streq(key, "bg_opacity")) {
    lua_pushnumber(L, block->styles.bg_opacity);
  } else if (streq(key, "border_color")) {
    lua_pushnumber(L, block->styles.border_color);
  } else if (streq(key, "border_opacity")) {
    lua_pushnumber(L, block->styles.border_opacity);
  } else if (streq(key, "border_width")) {
    lua_pushnumber(L, block->styles.border_width);
  } else if (streq(key, "insert")) {
    lua_pushcfunction(L, catnip_lua_widget_block_lua_insert);
  } else if (streq(key, "remove")) {
    lua_pushcfunction(L, catnip_lua_widget_block_lua_remove);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_block__newindex(lua_State* L)
{
  struct catnip_lua_widget_block* block = lua_touserdata(L, 1);

  if (lua_type(L, 2) == LUA_TNUMBER) {
    // TODO: need to set pointer backs
    lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
    lua_pushvalue(L, 3);
    lua_rawseti(L, -2, lua_tonumber(L, 2));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  }

  if (streq(key, "x")) {
    // TODO
  } else if (streq(key, "y")) {
    // TODO
  } else if (streq(key, "width")) {
    // TODO
  } else if (streq(key, "height")) {
    // TODO
  } else if (streq(key, "padding")) {
    block->styles.padding = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_layout(&block->base);
  } else if (streq(key, "padding_top")) {
    block->styles.padding_top = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_layout(&block->base);
  } else if (streq(key, "padding_left")) {
    block->styles.padding_left = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_layout(&block->base);
  } else if (streq(key, "padding_bottom")) {
    block->styles.padding_bottom = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_layout(&block->base);
  } else if (streq(key, "padding_right")) {
    block->styles.padding_right = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_layout(&block->base);
  } else if (streq(key, "radius")) {
    block->styles.radius = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "radius_top_left")) {
    block->styles.radius_top_left = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "radius_top_right")) {
    block->styles.radius_top_right = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "radius_bottom_left")) {
    block->styles.radius_bottom_left = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "radius_bottom_right")) {
    block->styles.radius_bottom_right = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "bg_color")) {
    block->styles.bg_color = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "bg_opacity")) {
    block->styles.bg_opacity = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "border_color")) {
    block->styles.border_color = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "border_opacity")) {
    block->styles.border_opacity = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  } else if (streq(key, "border_width")) {
    block->styles.border_width = luaL_checknumber(L, 3);
    catnip_lua_widget_base_request_draw(&block->base);
  }

  return 0;
}

static int
catnip_lua_widget_block__gc(lua_State* L)
{
  struct catnip_lua_widget_block* block = lua_touserdata(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct catnip_lua_widget_base* child = lua_touserdata(L, -1);
    child->root = NULL;
    child->parent = NULL;
    lua_pop(L, 1);
  }

  lua_pop(L, 1);
  luaL_unref(L, LUA_REGISTRYINDEX, block->children);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_block_mt[] = {
  {"__index", catnip_lua_widget_block__index},
  {"__newindex", catnip_lua_widget_block__newindex},
  {"__gc", catnip_lua_widget_block__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_block_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.block");
  luaL_setfuncs(L, catnip_lua_widget_block_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_block(lua_State* L)
{
  struct catnip_lua_widget_block* block =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_block));
  luaL_setmetatable(L, "catnip.widget.block");

  lua_newtable(L);
  block->children = luaL_ref(L, LUA_REGISTRYINDEX);

  block->base.root = NULL;
  block->base.parent = NULL;
  block->base.type = CATNIP_LUA_WIDGET_BLOCK;

  block->styles.bg_color = -1;
  block->styles.bg_opacity = 1;

  block->styles.border_color = -1;
  block->styles.border_opacity = 1;
  block->styles.border_width = 1;

  block->styles.radius_top_left = -1;
  block->styles.radius_top_right = -1;
  block->styles.radius_bottom_left = -1;
  block->styles.radius_bottom_right = -1;

  if (lua_type(L, 1) == LUA_TTABLE) {
    block->base.styles.x =
      lua_hasnumberfield(L, 1, "x") ? lua_popnumber(L) : -1;
    block->base.styles.y =
      lua_hasnumberfield(L, 1, "y") ? lua_popnumber(L) : -1;
    block->base.styles.width =
      lua_hasnumberfield(L, 1, "width") ? lua_popnumber(L) : -1;
    block->base.styles.height =
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

    lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
    lua_pushnil(L);

    while (lua_next(L, 1) != 0) {
      if (lua_type(L, -2) != LUA_TNUMBER) {
        lua_pop(L, 1);
      } else {
        struct catnip_lua_widget_base* child =
          lua_touserdata(L, -1); // TODO: typecheck value
        catnip_lua_widget_block_link(block, child);
        lua_rawseti(L, -3, lua_tonumber(L, -2));
      }
    }

    lua_pop(L, 1);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_block_pre_layout(
  lua_State* L,
  struct catnip_lua_widget_block* block
)
{
}

static void
catnip_lua_widget_block_post_layout(
  lua_State* L,
  struct catnip_lua_widget_block* block
)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct catnip_lua_widget_base* child = lua_touserdata(L, -1);

    child->computed.x = block->base.styles.x + child->styles.x;
    child->computed.y = block->base.styles.y + child->styles.y;
    child->computed.width = child->styles.width;
    child->computed.height = child->styles.height;

    lua_pop(L, 1);
  }

  lua_pop(L, 1);
}

void
catnip_lua_widget_block_layout(
  lua_State* L,
  struct catnip_lua_widget_block* block
)
{
  catnip_lua_widget_block_pre_layout(L, block);

  // TODO
  block->base.computed.x = block->base.styles.x;
  block->base.computed.y = block->base.styles.y;
  block->base.computed.width = block->base.styles.width;
  block->base.computed.height = block->base.styles.height;

  catnip_lua_widget_block_post_layout(L, block);
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_block_draw_self(
  struct catnip_lua_widget_block* block,
  cairo_t* cr
)
{
  cairo_save(cr);

  int radius_top_left = block->styles.radius_top_left == -1
    ? block->styles.radius
    : block->styles.radius_top_left;
  int radius_top_right = block->styles.radius_top_right == -1
    ? block->styles.radius
    : block->styles.radius_top_right;
  int radius_bottom_right = block->styles.radius_bottom_right == -1
    ? block->styles.radius
    : block->styles.radius_bottom_right;
  int radius_bottom_left = block->styles.radius_bottom_left == -1
    ? block->styles.radius
    : block->styles.radius_bottom_left;

  bool is_rounded = radius_top_left != 0 || radius_top_right != 0
    || radius_bottom_right != 0 || radius_bottom_left != 0;

  if (block->styles.bg_color != -1 && block->styles.bg_opacity > 0) {
    cairo_set_source_hexa(cr, block->styles.bg_color, block->styles.bg_opacity);

    if (is_rounded) {
      cairo_rounded_rectangle(
        cr,
        block->base.computed.x,
        block->base.computed.y,
        block->base.computed.width,
        block->base.computed.height,
        radius_top_left,
        radius_top_right,
        radius_bottom_right,
        radius_bottom_left
      );
    } else {
      cairo_rectangle(
        cr,
        block->base.computed.x,
        block->base.computed.y,
        block->base.computed.width,
        block->base.computed.height
      );
    }

    cairo_fill(cr);
  }

  if (block->styles.border_color != -1 && block->styles.border_opacity > 0
      && block->styles.border_width > 0) {
    double border_offset = (double) block->styles.border_width / 2;

    if (is_rounded) {
      cairo_rounded_rectangle(
        cr,
        block->base.computed.x + border_offset,
        block->base.computed.y + border_offset,
        block->base.computed.width - block->styles.border_width,
        block->base.computed.height - block->styles.border_width,
        radius_top_left,
        radius_top_right,
        radius_bottom_right,
        radius_bottom_left
      );
    } else {
      cairo_rectangle(
        cr,
        block->base.computed.x + border_offset,
        block->base.computed.y + border_offset,
        block->base.computed.width - block->styles.border_width,
        block->base.computed.height - block->styles.border_width
      );
    }

    cairo_set_line_width(cr, block->styles.border_width);
    cairo_set_source_hexa(
      cr,
      block->styles.border_color,
      block->styles.border_opacity
    );

    cairo_stroke(cr);
  }

  cairo_restore(cr);
}

static void
catnip_lua_widget_block_draw_children(
  lua_State* L,
  struct catnip_lua_widget_block* block,
  cairo_t* cr
)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, block->children);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct catnip_lua_widget_base* child = lua_touserdata(L, -1);

    switch (child->type) {
      case CATNIP_LUA_WIDGET_BLOCK:
        catnip_lua_widget_block_draw(
          L,
          (struct catnip_lua_widget_block*) child,
          cr
        );
        break;
      case CATNIP_LUA_WIDGET_IMG:
        // TODO
        break;
      case CATNIP_LUA_WIDGET_OUTPUT:
        // TODO
        break;
      case CATNIP_LUA_WIDGET_SVG:
        // TODO
        break;
      case CATNIP_LUA_WIDGET_TEXT:
        // TODO
        break;
      case CATNIP_LUA_WIDGET_WINDOW:
        // TODO
        break;
    }

    lua_pop(L, 1);
  }

  lua_pop(L, 1);
}

void
catnip_lua_widget_block_draw(
  lua_State* L,
  struct catnip_lua_widget_block* block,
  cairo_t* cr
)
{
  catnip_lua_widget_block_draw_self(block, cr);

  // TODO: inheritable properties

  catnip_lua_widget_block_draw_children(L, block, cr);
}
