#include "lua_keyboards.h"
#include "desktop/keyboards.h"
#include "desktop/lua_keyboard.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>

struct catnip_lua_keyboards* catnip_lua_keyboards = NULL;

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_keyboards_on(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.keyboards");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, catnip_lua_keyboards->subscriptions, event);

  return 1;
}

static int
catnip_lua_keyboards_emit(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.keyboards");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    catnip_lua_keyboards->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

static int
catnip_lua_keyboards__index(lua_State* L)
{
  int id = lua_tonumber(L, 2);
  const char* key = lua_tostring(L, 2);

  if (id != 0) {
    struct catnip_lua_keyboard* lua_keyboard = NULL;

    wl_list_for_each(lua_keyboard, &catnip_lua_keyboards->keyboards, link)
    {
      if (id == lua_keyboard->keyboard->id) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_keyboard->ref);
        return 1;
      }
    }

    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "on")) {
    lua_pushcfunction(L, catnip_lua_keyboards_on);
  } else if (streq(key, "emit")) {
    lua_pushcfunction(L, catnip_lua_keyboards_emit);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __call
// -----------------------------------------------------------------------------

static int
catnip_lua_keyboards__call(lua_State* L)
{
  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? catnip_lua_keyboards->keyboards.next
    : ((struct catnip_lua_keyboard*) lua_touserdata(L, 3))->link.next;

  if (link == &catnip_lua_keyboards->keyboards) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_lua_keyboard* lua_keyboard =
    wl_container_of(link, lua_keyboard, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_keyboard->ref);
  return 1;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_keyboards_mt[] = {
  {"__index", catnip_lua_keyboards__index},
  {"__call", catnip_lua_keyboards__call},
  {NULL, NULL}
};

void
catnip_lua_keyboards_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.keyboards");
  luaL_setfuncs(L, catnip_lua_keyboards_mt, 0);
  lua_pop(L, 1);

  catnip_lua_keyboards =
    lua_newuserdata(L, sizeof(struct catnip_lua_keyboards));
  luaL_setmetatable(L, "catnip.keyboards");

  catnip_lua_keyboards->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  catnip_lua_keyboards->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_list_init(&catnip_lua_keyboards->keyboards);
}

void
catnip_lua_keyboards_populate(lua_State* L)
{
  struct catnip_keyboard* keyboard = NULL;
  wl_list_for_each(keyboard, &catnip_keyboards, link)
  {
    catnip_lua_keyboard_create(L, keyboard);
  }
}
