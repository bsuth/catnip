#include "utils/lua.h"
#include "events/lua_events.h"
#include "output/lua_output_events.h"
#include "output/lua_output_mode.h"
#include "output/output_properties.h"
#include <glib.h>
#include <lauxlib.h>

lua_Ref lua_catnip_outputs = LUA_NOREF;

static void
lua_catnip_output_push_current_mode(lua_State* L, struct catnip_output* output)
{
  struct wlr_output_mode* current_mode = catnip_output_get_mode(output);

  lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.modes);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct wlr_output_mode** lua_mode = lua_touserdata(L, -1);

    if (current_mode == *lua_mode) {
      lua_remove(L, -3);
      lua_remove(L, -2);
      return;
    }

    lua_pop(L, 1);
  }

  lua_pop(L, 1);
  lua_pushnil(L);
}

static int
lua_catnip_output__index(lua_State* L)
{
  struct catnip_output** lua_output = lua_touserdata(L, 1);
  struct catnip_output* output = *lua_output;

  if (output == NULL) {
    lua_log(L, "attempt to index outdated output");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, catnip_output_get_x(output));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, catnip_output_get_y(output));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, catnip_output_get_width(output));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, catnip_output_get_height(output));
  } else if (g_str_equal(key, "refresh")) {
    lua_pushnumber(L, catnip_output_get_refresh(output));
  } else if (g_str_equal(key, "mode")) {
    lua_catnip_output_push_current_mode(L, output);
  } else if (g_str_equal(key, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.modes);
  } else if (g_str_equal(key, "scale")) {
    lua_pushnumber(L, catnip_output_get_scale(output));
  } else if (g_str_equal(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_output_subscribe);
  } else if (g_str_equal(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_output_unsubscribe);
  } else if (g_str_equal(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_output_publish);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_output__newindex(lua_State* L)
{
  struct catnip_output** lua_output = lua_touserdata(L, 1);
  struct catnip_output* output = *lua_output;

  if (output == NULL) {
    lua_log(L, "attempt to index outdated output");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_log(L, "invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    catnip_output_set_x(output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    catnip_output_set_y(output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    catnip_output_set_width(output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    catnip_output_set_height(output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "refresh")) {
    catnip_output_set_refresh(output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "mode")) {
    struct wlr_output_mode** lua_output_mode =
      luaL_checkudata(L, 3, "catnip.output.mode");
    catnip_output_set_mode(output, *lua_output_mode);
  } else if (g_str_equal(key, "scale")) {
    catnip_output_set_scale(output, luaL_checknumber(L, 3));
  } else {
    lua_log(L, "invalid key: %s", key);
  }

  return 0;
}

static int
lua_catnip_output__gc(lua_State* L)
{
  struct catnip_output** lua_output = lua_touserdata(L, 1);
  (*lua_output)->lua.userdata = NULL;
  return 0;
}

static const struct luaL_Reg lua_catnip_output_mt[] = {
  {"__index", lua_catnip_output__index},
  {"__newindex", lua_catnip_output__newindex},
  {"__gc", lua_catnip_output__gc},
  {NULL, NULL}
};

void
lua_catnip_output_destroy(lua_State* L, struct catnip_output* output)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_outputs);
  size_t lua_catnip_outputs_len = lua_objlen(L, -1);
  bool found_lua_output = false;

  for (int i = 1; i <= lua_catnip_outputs_len; ++i) {
    if (found_lua_output) {
      lua_rawgeti(L, -1, i);
      lua_rawseti(L, -2, i - 1);
    } else {
      lua_rawgeti(L, -1, i);
      found_lua_output = output == *(struct catnip_output**) lua_popuserdata(L);
    }
  }

  lua_pushnil(L);
  lua_rawseti(L, -2, lua_catnip_outputs_len);

  lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.ref);
  lua_catnip_events_call_publish(L, "output::destroy", 1);
  lua_catnip_output_call_publish(L, output, "destroy", 0);

  if (output->lua.userdata != NULL) {
    *(output->lua.userdata) = NULL;
  }

  luaL_unref(L, LUA_REGISTRYINDEX, output->lua.ref);
  luaL_unref(L, LUA_REGISTRYINDEX, output->lua.subscriptions);

  lua_catnip_output_modes_destroy(L, output);

  lua_pop(L, 1);
}

void
lua_catnip_output_create(lua_State* L, struct catnip_output* output)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_outputs);

  struct catnip_output** lua_output =
    lua_newuserdata(L, sizeof(struct catnip_output*));
  luaL_setmetatable(L, "catnip.output");

  *lua_output = output;
  output->lua.userdata = lua_output;

  lua_pushvalue(L, -1);
  output->lua.ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  output->lua.subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_catnip_output_modes_create(L, output);

  lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
  lua_pop(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.ref);
  lua_catnip_events_call_publish(L, "output::create", 1);
}

void
lua_catnip_output_init(lua_State* L)
{
  lua_newtable(L);
  lua_catnip_outputs = luaL_ref(L, LUA_REGISTRYINDEX);

  luaL_newmetatable(L, "catnip.output");
  luaL_setfuncs(L, lua_catnip_output_mt, 0);
  lua_pop(L, 1);

  lua_catnip_output_mode_init(L);

  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    lua_catnip_output_create(L, output);
  }
}
