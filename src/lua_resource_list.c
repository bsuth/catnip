#include "lua_resource_list.h"
#include "lua_resource.h"
#include <lauxlib.h>

static int
lua_catnip_resource_list__index(lua_State* L)
{
  struct catnip_lua_resource_list* lua_resource_list = lua_touserdata(L, 1);
  int id = luaL_checknumber(L, 2);

  struct catnip_lua_resource* lua_resource = NULL;
  wl_list_for_each(lua_resource, &lua_resource_list->head, link)
  {
    if (lua_resource->id == id) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
      return 1;
    }
  }

  lua_pushnil(L);
  return 1;
}

static int
lua_catnip_resource_list__call(lua_State* L)
{
  struct catnip_lua_resource_list* lua_resource_list = lua_touserdata(L, 1);

  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? lua_resource_list->head.next
    : ((struct catnip_lua_resource*) luaL_checkudata(L, 3, "catnip.resource"))
        ->link.next;

  if (link == &lua_resource_list->head) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_lua_resource* resource = wl_container_of(link, resource, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, resource->ref);
  return 1;
}

static const struct luaL_Reg lua_catnip_resource_list_mt[] = {
  {"__index", lua_catnip_resource_list__index},
  {"__call", lua_catnip_resource_list__call},
  {NULL, NULL}
};

struct catnip_lua_resource_list*
lua_catnip_resource_list_create(lua_State* L)
{
  struct catnip_lua_resource_list* lua_resource_list =
    lua_newuserdata(L, sizeof(struct catnip_lua_resource_list));
  luaL_setmetatable(L, "catnip.resource.list");

  wl_list_init(&lua_resource_list->head);
  lua_resource_list->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  return lua_resource_list;
}

void
lua_catnip_resource_list_destroy(
  lua_State* L,
  struct catnip_lua_resource_list* lua_resource_list
)
{
  luaL_unref(L, LUA_REGISTRYINDEX, lua_resource_list->ref);

  if (lua_resource_list->__destroy != NULL) {
    struct catnip_lua_resource* lua_resource = NULL;
    wl_list_for_each(lua_resource, &lua_resource_list->head, link)
    {
      lua_resource_list->__destroy(L, lua_resource);
    }
  }
}

void
lua_catnip_resource_list_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.resource.list");
  luaL_setfuncs(L, lua_catnip_resource_list_mt, 0);
  lua_pop(L, 1);
}
