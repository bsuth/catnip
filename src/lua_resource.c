#include "lua_resource.h"
#include "lua_events.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>

static int catnip_lua_resource_id_counter = 1;

static int
lua_catnip_resource_method_subscribe(lua_State* L)
{
  struct catnip_lua_resource* lua_resource =
    luaL_checkudata(L, 1, "catnip.resource");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, lua_resource->subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
lua_catnip_resource_method_unsubscribe(lua_State* L)
{
  struct catnip_lua_resource* lua_resource =
    luaL_checkudata(L, 1, "catnip.resource");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, lua_resource->subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
lua_catnip_resource_method_publish(lua_State* L)
{
  struct catnip_lua_resource* lua_resource =
    luaL_checkudata(L, 1, "catnip.resource");
  const char* event = luaL_checkstring(L, 2);
  lua_catnip_resource_publish(L, lua_resource, event, lua_gettop(L) - 2);
  return 0;
}

static int
lua_catnip_resource__index(lua_State* L)
{
  struct catnip_lua_resource* lua_resource = lua_touserdata(L, 1);

  if (lua_resource->data == NULL) {
    lua_log_error(L, "attempt to index outdated resource");
    lua_pushnil(L);
  } else if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "id")) {
    lua_pushnumber(L, lua_resource->id);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_resource_method_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_resource_method_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_resource_method_publish);
  } else if (lua_resource->__index == NULL || !lua_resource->__index(L, lua_resource, key)) {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_resource__newindex(lua_State* L)
{
  struct catnip_lua_resource* lua_resource = lua_touserdata(L, 1);

  if (lua_resource->data == NULL) {
    lua_log_error(L, "attempt to index outdated resource");
    return 0;
  } else if (lua_type(L, 2) != LUA_TSTRING) {
    lua_log_warning(
      L,
      "attempt to index resource with type '%s'",
      luaL_typename(L, 2)
    );
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  bool success = !streq(key, "id") && lua_resource->__newindex != NULL
    && lua_resource->__newindex(L, lua_resource, key);

  if (!success) {
    lua_log_warning(L, "attempt set unknown index '%s'", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_resource_mt[] = {
  {"__index", lua_catnip_resource__index},
  {"__newindex", lua_catnip_resource__newindex},
  {NULL, NULL}
};

struct catnip_lua_resource*
lua_catnip_resource_create(lua_State* L)
{
  struct catnip_lua_resource* lua_resource =
    lua_newuserdata(L, sizeof(struct catnip_lua_resource));
  luaL_setmetatable(L, "catnip.resource");

  lua_resource->id = catnip_lua_resource_id_counter++;
  lua_resource->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_resource->data = NULL;
  lua_resource->name = NULL;
  lua_resource->__index = NULL;
  lua_resource->__newindex = NULL;

  lua_newtable(L);
  lua_resource->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  return lua_resource;
}

void
lua_catnip_resource_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  lua_resource->data = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, lua_resource->ref);
  luaL_unref(L, LUA_REGISTRYINDEX, lua_resource->subscriptions);
}

void*
lua_catnip_resource_checkname(lua_State* L, int index, const char* name)
{
  struct catnip_lua_resource* lua_resource =
    luaL_checkudata(L, index, "catnip.resource");

  if (!streq(name, lua_resource->name)) {
    luaL_typerror(L, index, name);
  }

  return lua_resource->data;
}

void
lua_catnip_resource_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
)
{
  lua_catnip_events_publish(L, lua_resource->subscriptions, event, nargs);

  if (lua_resource->name != NULL) {
    char* global_event = strfmt("%s::%s", lua_resource->name, event);

    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
    lua_insert(L, -1 - nargs);

    lua_catnip_events_publish(
      L,
      lua_catnip_subscriptions,
      global_event,
      nargs + 1
    );

    free(global_event);
    lua_remove(L, -1 - nargs);
  };
}

void
lua_catnip_resource_init(lua_State* L)
{
  catnip_lua_resource_id_counter = 1;
  luaL_newmetatable(L, "catnip.resource");
  luaL_setfuncs(L, lua_catnip_resource_mt, 0);
  lua_pop(L, 1);
}
