#include "box.h"
#include "api/ui/ui.h"
#include "lua.h"
#include "ui/box.h"
#include "ui/types.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

struct api_ui_box {
  struct ui_box* ui_box;
};

// -----------------------------------------------------------------------------
// Metatable: catnip.ui.box
// -----------------------------------------------------------------------------

static int
api_ui_box__index(lua_State* L)
{
  struct api_ui_box* api_ui_box = lua_touserdata(L, 1);
  struct ui_box* ui_box = api_ui_box->ui_box;

  if (ui_box == NULL) {
    log_warning("cannot get field of expired box");
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
    lua_pushnumber(L, ui_box_get_width(ui_box));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, ui_box_get_height(ui_box));
  } else if (g_str_equal(key, "background")) {
    lua_pushnumber(L, ui_box_get_background(ui_box));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_ui_box__newindex(lua_State* L)
{
  struct api_ui_box* api_ui_box = lua_touserdata(L, 1);
  struct ui_box* ui_box = api_ui_box->ui_box;

  if (ui_box == NULL) {
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
    ui_box_set_width(ui_box, luaL_checknumber(L, 3), UI_SIZE_PX);
  } else if (g_str_equal(key, "height")) {
    ui_box_set_height(ui_box, luaL_checknumber(L, 3), UI_SIZE_PX);
  } else if (g_str_equal(key, "background")) {
    ui_box_set_background(ui_box, luaL_checknumber(L, 3));
  } else {
    log_warning("invalid key: %s", key);
  }

  return 0;
}

static const struct luaL_Reg api_ui_box_metatable[] = {
  {"__index", api_ui_box__index},
  {"__newindex", api_ui_box__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static int
api_create_ui_box(lua_State* L)
{
  struct api_ui_box* api_ui_box = lua_newuserdata(L, sizeof(struct api_ui_box));
  luaL_setmetatable(L, "catnip.ui.box");

  struct ui_box* ui_box = create_ui_box();
  api_ui_box->ui_box = ui_box;

  if (lua_type(L, 1) == LUA_TTABLE) {
    lua_getfield(L, 1, "width");

    if (lua_type(L, -1) == LUA_TNUMBER) {
      ui_box_set_width(ui_box, luaL_checknumber(L, 3), UI_SIZE_PX);
    }

    lua_pop(L, 1);
  }

  render_ui_box(ui_box);
  return 1;
}

void
init_api_ui_box(lua_State* L)
{
  luaL_newmetatable(L, "catnip.ui.box");
  luaL_setfuncs(L, api_ui_box_metatable, 0);
  lua_pop(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_ui);
  lua_pushcfunction(L, api_create_ui_box);
  lua_setfield(L, -2, "box");
  lua_pop(L, 1);
}
