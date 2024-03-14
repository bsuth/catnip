#include "lua_event.h"
#include <lauxlib.h>

static int
lua_catnip_event__index(lua_State* L)
{
  struct catnip_lua_event* lua_event = lua_touserdata(L, 1);

  if (lua_event->data == NULL) {
    lua_log_error(L, "attempt to index outdated event");
    lua_pushnil(L);
  } else if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
  } else if (lua_event->__index == NULL) {
    lua_pushnil(L);
  } else if (!lua_event->__index(L, lua_event, lua_tostring(L, 2))) {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_event__newindex(lua_State* L)
{
  struct catnip_lua_event* lua_event = lua_touserdata(L, 1);

  if (lua_event->data == NULL) {
    lua_log_error(L, "attempt to index outdated event");
    return 0;
  } else if (lua_type(L, 2) != LUA_TSTRING) {
    lua_log_warning(
      L,
      "attempt to index event with type '%s'",
      luaL_typename(L, 2)
    );
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  bool success =
    lua_event->__newindex != NULL && lua_event->__newindex(L, lua_event, key);

  if (!success) {
    lua_log_warning(L, "attempt set unknown index '%s'", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_event_mt[] = {
  {"__index", lua_catnip_event__index},
  {"__newindex", lua_catnip_event__newindex},
  {NULL, NULL}
};

struct catnip_lua_event*
lua_catnip_event_create(lua_State* L)
{
  struct catnip_lua_event* lua_event =
    lua_newuserdata(L, sizeof(struct catnip_lua_event));
  luaL_setmetatable(L, "catnip.event");
  lua_event->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  return lua_event;
}

void
lua_catnip_event_destroy(lua_State* L, struct catnip_lua_event* lua_event)
{
  lua_event->data = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, lua_event->ref);
}

void
lua_catnip_event_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.event");
  luaL_setfuncs(L, lua_catnip_event_mt, 0);
  lua_pop(L, 1);
}
