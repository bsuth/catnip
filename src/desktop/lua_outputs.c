#include "lua_outputs.h"
#include "desktop/lua_output.h"
#include "desktop/outputs.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>

struct catnip_lua_outputs* catnip_lua_outputs = NULL;

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_outputs_subscribe(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.outputs");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, catnip_lua_outputs->subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
catnip_lua_outputs_unsubscribe(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.outputs");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_unsubscribe(L, catnip_lua_outputs->subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
catnip_lua_outputs_publish(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.outputs");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    catnip_lua_outputs->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

static int
catnip_lua_outputs__index(lua_State* L)
{
  int id = lua_tonumber(L, 2);
  const char* key = lua_tostring(L, 2);

  if (id != 0) {
    struct catnip_lua_output* lua_output = NULL;

    wl_list_for_each(lua_output, &catnip_lua_outputs->outputs, link)
    {
      if (id == lua_output->output->id) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_output->ref);
        return 1;
      }
    }

    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, catnip_lua_outputs_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, catnip_lua_outputs_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, catnip_lua_outputs_publish);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __call
// -----------------------------------------------------------------------------

static int
catnip_lua_outputs__call(lua_State* L)
{
  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? catnip_lua_outputs->outputs.next
    : ((struct catnip_lua_output*) lua_touserdata(L, 3))->link.next;

  if (link == &catnip_lua_outputs->outputs) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_lua_output* lua_output =
    wl_container_of(link, lua_output, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_output->ref);
  return 1;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_outputs_mt[] = {
  {"__index", catnip_lua_outputs__index},
  {"__call", catnip_lua_outputs__call},
  {NULL, NULL}
};

void
catnip_lua_outputs_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.outputs");
  luaL_setfuncs(L, catnip_lua_outputs_mt, 0);
  lua_pop(L, 1);

  catnip_lua_outputs = lua_newuserdata(L, sizeof(struct catnip_lua_outputs));
  luaL_setmetatable(L, "catnip.outputs");

  catnip_lua_outputs->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  catnip_lua_outputs->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_list_init(&catnip_lua_outputs->outputs);
}

void
catnip_lua_outputs_populate(lua_State* L)
{
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    catnip_lua_output_create(L, output);
  }
}
