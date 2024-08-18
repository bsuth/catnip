#include "lua_windows.h"
#include "desktop/lua_window.h"
#include "desktop/windows.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>

struct catnip_lua_windows* catnip_lua_windows = NULL;

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_windows_subscribe(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.windows");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, catnip_lua_windows->subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
catnip_lua_windows_unsubscribe(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.windows");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_unsubscribe(L, catnip_lua_windows->subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
catnip_lua_windows_publish(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.windows");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    catnip_lua_windows->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

static int
catnip_lua_windows__index(lua_State* L)
{
  int id = lua_tonumber(L, 2);
  const char* key = lua_tostring(L, 2);

  if (id != 0) {
    struct catnip_lua_window* lua_window = NULL;

    wl_list_for_each(lua_window, &catnip_lua_windows->windows, link)
    {
      if (id == lua_window->window->id) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_window->ref);
        return 1;
      }
    }

    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, catnip_lua_windows_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, catnip_lua_windows_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, catnip_lua_windows_publish);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __call
// -----------------------------------------------------------------------------

static int
catnip_lua_windows__call(lua_State* L)
{
  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? catnip_lua_windows->windows.next
    : ((struct catnip_lua_window*) lua_touserdata(L, 3))->link.next;

  if (link == &catnip_lua_windows->windows) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_lua_window* lua_window =
    wl_container_of(link, lua_window, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_window->ref);
  return 1;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_windows_mt[] = {
  {"__index", catnip_lua_windows__index},
  {"__call", catnip_lua_windows__call},
  {NULL, NULL}
};

void
catnip_lua_windows_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.windows");
  luaL_setfuncs(L, catnip_lua_windows_mt, 0);
  lua_pop(L, 1);

  catnip_lua_windows = lua_newuserdata(L, sizeof(struct catnip_lua_windows));
  luaL_setmetatable(L, "catnip.windows");

  catnip_lua_windows->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  catnip_lua_windows->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_list_init(&catnip_lua_windows->windows);
}

void
catnip_lua_windows_populate(lua_State* L)
{
  struct catnip_window* window = NULL;
  wl_list_for_each(window, &catnip_windows, link)
  {
    catnip_lua_window_create(L, window);
  }
}
