#include "lua_widget_base.h"
#include "compositor/event_loop.h"
#include "extensions/string.h"
#include "widget/lua_widget_root.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_base_setup(lua_State* L, struct catnip_lua_widget_base* base)
{
  base->parent = NULL;

  base->event_sources.request_layout = NULL;
  base->event_sources.request_draw = NULL;

  base->bounding_box.x = 0;
  base->bounding_box.y = 0;
  base->bounding_box.width = 0;
  base->bounding_box.height = 0;
}

void
catnip_lua_widget_base_cleanup(
  lua_State* L,
  struct catnip_lua_widget_base* base
)
{
  if (base->event_sources.request_layout != NULL) {
    wl_event_source_remove(base->event_sources.request_layout);
  }

  if (base->event_sources.request_draw != NULL) {
    wl_event_source_remove(base->event_sources.request_draw);
  }
}

bool
catnip_lua_widget_base__index(
  lua_State* L,
  struct catnip_lua_widget_base* base,
  const char* key
)
{
  if (streq(key, "x")) {
    lua_pushnumber(L, base->bounding_box.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, base->bounding_box.y);
  } else if (streq(key, "width")) {
    lua_pushnumber(L, base->bounding_box.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, base->bounding_box.height);
  } else if (streq(key, "type")) {
    switch (base->type) {
      case CATNIP_LUA_WIDGET_BLOCK:
        lua_pushstring(L, "block");
        return true;
      case CATNIP_LUA_WIDGET_PNG:
        lua_pushstring(L, "png");
        return true;
      case CATNIP_LUA_WIDGET_ROOT:
        lua_pushstring(L, "root");
        return true;
      case CATNIP_LUA_WIDGET_SVG:
        lua_pushstring(L, "svg");
        return true;
      case CATNIP_LUA_WIDGET_TEXT:
        lua_pushstring(L, "text");
        return true;
      default:
        lua_pushstring(L, "");
        return true;
    }
  } else {
    return false;
  }

  return true;
}

bool
catnip_lua_widget_base__newindex(
  lua_State* L,
  struct catnip_lua_widget_base* base,
  const char* key
)
{
  if (streq(key, "x")) {
    base->bounding_box.x = luaL_checknumber(L, 3);
  } else if (streq(key, "y")) {
    base->bounding_box.y = luaL_checknumber(L, 3);
  } else if (streq(key, "width")) {
    base->bounding_box.width = luaL_checknumber(L, 3);
  } else if (streq(key, "height")) {
    base->bounding_box.height = luaL_checknumber(L, 3);
  } else {
    return false;
  }

  return true;
}

enum catnip_lua_widget_type
catnip_lua_widget_base_type(lua_State* L, int idx)
{
  lua_getmetatable(L, idx);

  luaL_getmetatable(L, "catnip.widget.block");

  if (lua_equal(L, -1, -2)) {
    lua_pop(L, 2);
    return CATNIP_LUA_WIDGET_BLOCK;
  } else {
    lua_pop(L, 1);
  }

  luaL_getmetatable(L, "catnip.widget.png");

  if (lua_equal(L, -1, -2)) {
    lua_pop(L, 2);
    return CATNIP_LUA_WIDGET_PNG;
  } else {
    lua_pop(L, 1);
  }

  luaL_getmetatable(L, "catnip.widget.root");

  if (lua_equal(L, -1, -2)) {
    lua_pop(L, 2);
    return CATNIP_LUA_WIDGET_ROOT;
  } else {
    lua_pop(L, 1);
  }

  luaL_getmetatable(L, "catnip.widget.svg");

  if (lua_equal(L, -1, -2)) {
    lua_pop(L, 2);
    return CATNIP_LUA_WIDGET_SVG;
  } else {
    lua_pop(L, 1);
  }

  luaL_getmetatable(L, "catnip.widget.text");

  if (lua_equal(L, -1, -2)) {
    lua_pop(L, 2);
    return CATNIP_LUA_WIDGET_TEXT;
  } else {
    lua_pop(L, 1);
  }

  luaL_getmetatable(L, "catnip.widget.tostring");

  if (lua_equal(L, -1, -2)) {
    lua_pop(L, 2);
    return CATNIP_LUA_WIDGET_TOSTRING;
  } else {
    lua_pop(L, 1);
  }

  lua_pop(L, 1);
  return CATNIP_LUA_WIDGET_NONE;
}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------

static void
__catnip_lua_widget_base_request_layout(void* data)
{
  struct catnip_lua_widget_base* base = data;
  struct catnip_lua_widget_base* ancestor = base->parent;

  // TODO: stop at first parent not dependent on children sizes

  while (ancestor != NULL && ancestor->type != CATNIP_LUA_WIDGET_ROOT) {
    ancestor = ancestor->parent;
  }

  if (ancestor != NULL) {
    catnip_lua_widget_root_request_layout(
      (struct catnip_lua_widget_root*) ancestor
    );
  }

  base->event_sources.request_layout = NULL;
}

void
catnip_lua_widget_base_request_layout(
  lua_State* L,
  struct catnip_lua_widget_base* base
)
{
  if (base->event_sources.request_layout != NULL) {
    return; // already queued
  }

  base->event_sources.request_layout = wl_event_loop_add_idle(
    catnip_event_loop,
    __catnip_lua_widget_base_request_layout,
    base
  );
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------

static void
__catnip_lua_widget_base_request_draw(void* data)
{
  struct catnip_lua_widget_base* base = data;
  struct catnip_lua_widget_base* ancestor = base->parent;

  while (ancestor != NULL && ancestor->type != CATNIP_LUA_WIDGET_ROOT) {
    ancestor = ancestor->parent;
  }

  if (ancestor != NULL) {
    catnip_lua_widget_root_request_draw(
      (struct catnip_lua_widget_root*) ancestor
    );
  }

  base->event_sources.request_draw = NULL;
}

void
catnip_lua_widget_base_request_draw(
  lua_State* L,
  struct catnip_lua_widget_base* base
)
{
  if (base->event_sources.request_draw != NULL) {
    return; // already queued
  }

  base->event_sources.request_draw = wl_event_loop_add_idle(
    catnip_event_loop,
    __catnip_lua_widget_base_request_draw,
    base
  );
}
