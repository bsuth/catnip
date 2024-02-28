#include "lua_output.h"
#include "lua_events.h"
#include "output/lua_output_methods.h"
#include "output/lua_output_mode.h"
#include "output/output_properties.h"
#include "utils/lua.h"
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
    lua_log_error(L, "attempt to index outdated userdata");
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "id")) {
    lua_pushnumber(L, output->id);
  } else if (g_str_equal(key, "x")) {
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
    lua_pushcfunction(L, lua_catnip_output_method_subscribe);
  } else if (g_str_equal(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_output_method_unsubscribe);
  } else if (g_str_equal(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_output_method_publish);
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
    lua_log_error(L, "attempt to index outdated userdata");
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
    lua_log_error(L, "unknown userdata field (%s)", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_output_mt[] = {
  {"__index", lua_catnip_output__index},
  {"__newindex", lua_catnip_output__newindex},
  {NULL, NULL}
};

void
lua_catnip_output_destroy(lua_State* L, struct catnip_output* output)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_outputs);
  lua_pushnil(L);
  lua_rawseti(L, -2, output->id);
  lua_pop(L, 1);

  lua_catnip_output_publish(L, output, "destroy", 0);

  *(output->lua.userdata) = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, output->lua.ref);
  luaL_unref(L, LUA_REGISTRYINDEX, output->lua.subscriptions);

  lua_catnip_output_modes_destroy(L, output);
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

  lua_rawseti(L, -2, output->id);
  lua_pop(L, 1);

  lua_catnip_output_publish(L, output, "create", 0);
}

void
lua_catnip_output_publish(
  lua_State* L,
  struct catnip_output* output,
  const char* event,
  int nargs
)
{
  lua_catnip_events_publish(L, output->lua.subscriptions, event, nargs);

  gchar* global_event = g_strconcat("output::", event, NULL);
  lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.ref);
  lua_insert(L, -1 - nargs);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    global_event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
  g_free(global_event);
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
