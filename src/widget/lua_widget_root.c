#include "lua_widget_root.h"
#include "extensions/string.h"
#include "widget/lua_widget_base.h"
#include "widget/widget_root.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_root__index(lua_State* L, struct catnip_widget_base* base)
{
  struct catnip_widget_root* root = base->data;
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, root->wlr.scene_buffer->node.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, root->wlr.scene_buffer->node.y);
  } else if (streq(key, "width")) {
    lua_pushnumber(L, root->wlr.buffer.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, root->wlr.buffer.width);
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, root->wlr.scene_buffer->node.enabled);
  } else if (streq(key, "widget")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, root->lua_widget);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_root__newindex(lua_State* L, struct catnip_widget_base* base)
{
  struct catnip_widget_root* root = base->data;
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return;
  }

  if (streq(key, "x")) {
    wlr_scene_node_set_position(
      &root->wlr.scene_buffer->node,
      luaL_checkinteger(L, 3),
      root->wlr.scene_buffer->node.y
    );
  } else if (streq(key, "y")) {
    wlr_scene_node_set_position(
      &root->wlr.scene_buffer->node,
      root->wlr.scene_buffer->node.x,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "width")) {
    catnip_widget_root_request_resize(
      root,
      luaL_checkinteger(L, 3),
      root->request.height
    );
  } else if (streq(key, "height")) {
    catnip_widget_root_request_resize(
      root,
      root->request.width,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "visible")) {
    wlr_scene_node_set_enabled(
      &root->wlr.scene_buffer->node,
      lua_toboolean(L, 3)
    );
  } else if (streq(key, "widget")) {
    luaL_unref(L, LUA_REGISTRYINDEX, root->lua_widget);

    if (root->widget != NULL) {
      root->widget->root = NULL;
      root->widget->parent = NULL;
    }

    if (lua_type(L, 3) == LUA_TNIL) {
      root->widget = NULL;
      root->lua_widget = LUA_NOREF;
    } else {
      root->widget = luaL_checkudata(L, 3, "catnip.widget.base");
      root->widget->root = root;
      root->widget->parent = base;
      lua_pushvalue(L, 3);
      root->lua_widget = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    catnip_widget_root_request_render(root);
  }
}

// -----------------------------------------------------------------------------
// __gc
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_root__gc(lua_State* L, struct catnip_widget_base* base)
{
  struct catnip_widget_root* root = base->data;

  if (root->widget != NULL) {
    root->widget->root = NULL;
    root->widget->parent = NULL;
  }

  luaL_unref(L, LUA_REGISTRYINDEX, root->lua_widget);
  catnip_widget_root_destroy(root);
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static struct catnip_lua_widget_base_mt catnip_lua_widget_root_mt = {
  .__index = catnip_lua_widget_root__index,
  .__newindex = catnip_lua_widget_root__newindex,
  .__gc = catnip_lua_widget_root__gc,
};

int
catnip_lua_widget_root(lua_State* L)
{
  bool has_init_table = lua_type(L, 1) == LUA_TTABLE;

  struct catnip_widget_root* root = catnip_widget_root_create(
    has_init_table && lua_hasnumberfield(L, 1, "width") ? lua_popnumber(L) : 0,
    has_init_table && lua_hasnumberfield(L, 1, "height") ? lua_popnumber(L) : 0
  );

  struct catnip_widget_base* base = catnip_lua_widget_base(L);
  base->parent = NULL;
  base->data = root;
  base->type = CATNIP_WIDGET_ROOT;
  base->mt = &catnip_lua_widget_root_mt;

  if (has_init_table) {
    wlr_scene_node_set_position(
      &root->wlr.scene_buffer->node,
      lua_hasnumberfield(L, 1, "x") ? lua_popnumber(L) : 0,
      lua_hasnumberfield(L, 1, "y") ? lua_popnumber(L) : 0
    );

    wlr_scene_node_set_enabled(
      &root->wlr.scene_buffer->node,
      lua_hasbooleanfield(L, 1, "visible") ? lua_popboolean(L) : true
    );

    if (lua_hasuserdatafield(L, 1, "widget")) {
      root->widget = luaL_checkudata(L, -1, "catnip.widget.base");
      root->widget->root = root;
      root->widget->parent = base;
      root->lua_widget = luaL_ref(L, LUA_REGISTRYINDEX);
      catnip_widget_root_request_render(root);
    }
  }

  return 1;
}
