#include "lua_output_mode.h"
#include "config/config.h"
#include "output/properties.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>

static int
lua_catnip_output_mode__index(lua_State* L)
{
  struct wlr_output_mode** lua_mode = lua_touserdata(L, 1);
  struct wlr_output_mode* mode = *lua_mode;

  if (mode == NULL) {
    char* error = lua_error_msg(L, "attempt to index outdated output mode");
    log_error("%s", error);
    free(error);
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "width")) {
    lua_pushnumber(L, mode->width);
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, mode->height);
  } else if (g_str_equal(key, "refresh")) {
    lua_pushnumber(L, mode->refresh);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static const struct luaL_Reg lua_catnip_output_mode_mt[] = {
  {"__index", lua_catnip_output_mode__index},
  {NULL, NULL}
};

void
lua_catnip_output_modes_destroy(lua_State* L, struct catnip_output* output)
{
  luaL_unref(L, LUA_REGISTRYINDEX, output->lua.modes);
}

void
lua_catnip_output_modes_create(lua_State* L, struct catnip_output* output)
{
  // TODO: make this table read only from lua
  lua_newtable(L);

  struct wlr_output_mode* mode;
  struct wl_list* modes = catnip_output_get_modes(output);
  int i = 0;

  wl_list_for_each(mode, modes, link)
  {
    struct wlr_output_mode** lua_mode =
      lua_newuserdata(L, sizeof(struct wlr_output_mode*));
    luaL_setmetatable(L, "catnip.output.mode");
    *lua_mode = mode;
    lua_rawseti(L, -2, ++i);
  }

  output->lua.modes = luaL_ref(L, LUA_REGISTRYINDEX);
}

void
lua_catnip_output_mode_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.output.mode");
  luaL_setfuncs(L, lua_catnip_output_mode_mt, 0);
  lua_pop(L, 1);
}
