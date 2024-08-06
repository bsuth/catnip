#include "lua_object_list.h"
#include "lua_objects/lua_object.h"
#include "objects/vector.h"
#include <lauxlib.h>

struct _catnip_lua_object_list {
  lua_Ref ref;
  catnip_vector_t* vector;
};

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_object_list__index(lua_State* L)
{
  struct _catnip_lua_object_list* lua_object_list = lua_touserdata(L, 1);

  // TODO allow index by id
  // TODO allow pubsub

  return 1;
}

static int
catnip_lua_object_list__iter(lua_State* L)
{
  struct _catnip_lua_object_list* lua_object_list =
    lua_touserdata(L, lua_upvalueindex(1));

  size_t index = lua_tonumber(L, lua_upvalueindex(2));

  if (index < catnip_vector_get_length(lua_object_list->vector)) {
    catnip_lua_object_t* lua_object =
      catnip_vector_at(lua_object_list->vector, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_object_get_ref(L, lua_object));
  } else {
    lua_pushnil(L);
  }

  lua_pushnumber(L, index + 1);
  lua_replace(L, lua_upvalueindex(2));

  return 1;
}

static int
catnip_lua_object_list__call(lua_State* L)
{
  struct _catnip_lua_object_list* lua_object_list = lua_touserdata(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_object_list->ref);
  lua_pushnumber(L, 0);
  lua_pushcclosure(L, catnip_lua_object_list__iter, 2);

  return 1;
}

static int
catnip_lua_object_list__gc(lua_State* L)
{
  struct _catnip_lua_object_list* lua_object_list = lua_touserdata(L, 1);

  catnip_vector_destroy(lua_object_list->vector);
  lua_object_list->vector = NULL;

  return 0;
}

// TODO: register metatable
static const struct luaL_Reg catnip_lua_object_list_mt[] = {
  {"__index", catnip_lua_object_list__index},
  {"__call", catnip_lua_object_list__call},
  {"__gc", catnip_lua_object_list__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct _catnip_lua_object_list*
catnip_lua_object_list_create(lua_State* L)
{
  struct _catnip_lua_object_list* lua_object_list =
    lua_newuserdata(L, sizeof(struct _catnip_lua_object_list));
  luaL_setmetatable(L, "catnip.object.list");

  lua_object_list->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_object_list->vector = catnip_vector_create(5);

  return lua_object_list;
}

void
catnip_lua_object_list_destroy(
  lua_State* L,
  struct _catnip_lua_object_list* lua_object_list
)
{
  luaL_unref(L, LUA_REGISTRYINDEX, lua_object_list->ref);
}

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

lua_Ref
catnip_lua_object_list_get_ref(
  lua_State* L,
  struct _catnip_lua_object_list* lua_object_list
)
{
  return lua_object_list->ref;
}
