#include "root.h"
#include "api/ui/properties.h"
#include "api/ui/ui.h"
#include "lua.h"
#include "ui/root.h"
#include "ui/types.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

struct api_ui_root {
  struct ui_root* ui_root;
};

// -----------------------------------------------------------------------------
// Metatable: catnip.ui.root
// -----------------------------------------------------------------------------

static int
api_ui_root__index(lua_State* L)
{
  struct api_ui_root* api_ui_root = lua_touserdata(L, 1);
  struct ui_root* ui_root = api_ui_root->ui_root;

  if (ui_root == NULL) {
    log_warning("cannot get field of expired root");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TNUMBER && key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "width")) {
    lua_get_ui_dimension(L, &ui_root->width);
  } else if (g_str_equal(key, "height")) {
    lua_get_ui_dimension(L, &ui_root->height);
  } else if (g_str_equal(key, "computed_width")) {
    lua_pushnumber(L, ui_root->computed_width);
  } else if (g_str_equal(key, "computed_height")) {
    lua_pushnumber(L, ui_root->computed_height);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_ui_root__newindex(lua_State* L)
{
  struct api_ui_root* api_ui_root = lua_touserdata(L, 1);
  struct ui_root* ui_root = api_ui_root->ui_root;

  if (ui_root == NULL) {
    log_warning("cannot get field of expired root");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TNUMBER && key_type != LUA_TSTRING) {
    log_warning("invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  // TODO: handle number key

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "width")) {
    lua_set_ui_dimension(L, &ui_root->width, "width");
  } else if (g_str_equal(key, "height")) {
    lua_set_ui_dimension(L, &ui_root->height, "height");
  } else {
    log_warning("invalid key: %s", key);
  }

  return 0;
}

static const struct luaL_Reg api_ui_root_metatable[] = {
  {"__index", api_ui_root__index},
  {"__newindex", api_ui_root__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static int
api_create_ui_root(lua_State* L)
{
  struct api_ui_root* api_ui_root =
    lua_newuserdata(L, sizeof(struct api_ui_root));
  luaL_setmetatable(L, "catnip.ui.root");

  struct ui_root* ui_root = create_ui_root();
  api_ui_root->ui_root = ui_root;

  if (lua_type(L, 1) == LUA_TTABLE) {
    lua_getfield(L, 1, "width");
    lua_set_ui_dimension(L, &ui_root->width, "width");
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    lua_set_ui_dimension(L, &ui_root->height, "height");
    lua_pop(L, 1);
  }

  ui_root_refresh(ui_root);
  // TODO
  // render_ui_root(ui_root);

  return 1;
}

void
init_api_ui_root(lua_State* L)
{
  luaL_newmetatable(L, "catnip.ui.root");
  luaL_setfuncs(L, api_ui_root_metatable, 0);
  lua_pop(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_ui);
  lua_pushcfunction(L, api_create_ui_root);
  lua_setfield(L, -2, "root");
  lua_pop(L, 1);
}
