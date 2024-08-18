#include "lua_cursor.h"
#include "desktop/cursor.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

struct catnip_lua_cursor* catnip_lua_cursor = NULL;

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_cursor_subscribe(lua_State* L)
{
  struct catnip_lua_cursor* lua_cursor = luaL_checkudata(L, 1, "catnip.cursor");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, lua_cursor->subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
catnip_lua_cursor_unsubscribe(lua_State* L)
{
  struct catnip_lua_cursor* lua_cursor = luaL_checkudata(L, 1, "catnip.cursor");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_unsubscribe(L, lua_cursor->subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
catnip_lua_cursor_publish(lua_State* L)
{
  struct catnip_lua_cursor* lua_cursor = luaL_checkudata(L, 1, "catnip.cursor");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    lua_cursor->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

static int
catnip_lua_cursor__index(lua_State* L)
{
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, catnip_lua_cursor_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, catnip_lua_cursor_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, catnip_lua_cursor_publish);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, catnip_cursor->x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, catnip_cursor->y);
  } else if (streq(key, "name")) {
    lua_pushstring(L, catnip_xcursor_name);
  } else if (streq(key, "size")) {
    lua_pushnumber(L, catnip_xcursor_manager->size);
  } else if (streq(key, "theme")) {
    lua_pushstring(L, catnip_xcursor_manager->name);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static int
catnip_lua_cursor__newindex(lua_State* L)
{
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  }

  if (streq(key, "x")) {
    catnip_cursor_move(luaL_checknumber(L, 3), catnip_cursor->y);
  } else if (streq(key, "y")) {
    catnip_cursor_move(catnip_cursor->x, luaL_checknumber(L, 3));
  } else if (streq(key, "name")) {
    const char* new_name = luaL_checkstring(L, 3);

    if (catnip_xcursor_name != NULL) {
      free(catnip_xcursor_name);
      catnip_xcursor_name = NULL;
    }

    if (new_name == NULL) {
      wlr_cursor_unset_image(catnip_cursor);
    } else {
      catnip_xcursor_name = strdup(new_name);
      wlr_cursor_set_xcursor(catnip_cursor, catnip_xcursor_manager, new_name);
    }
  } else if (streq(key, "size")) {
    catnip_cursor_xcursor(catnip_xcursor_manager->name, luaL_checknumber(L, 3));
  } else if (streq(key, "theme")) {
    catnip_cursor_xcursor(luaL_checkstring(L, 3), catnip_xcursor_manager->size);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_cursor_mt[] = {
  {"__index", catnip_lua_cursor__index},
  {"__newindex", catnip_lua_cursor__newindex},
  {NULL, NULL}
};

void
catnip_lua_cursor_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.cursor");
  luaL_setfuncs(L, catnip_lua_cursor_mt, 0);
  lua_pop(L, 1);

  catnip_lua_cursor = lua_newuserdata(L, sizeof(struct catnip_lua_cursor));
  luaL_setmetatable(L, "catnip.cursor");

  catnip_lua_cursor->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  catnip_lua_cursor->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}

void
catnip_lua_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
)
{
  lua_pushnumber(L, event->button);

  catnip_lua_events_publish(
    L,
    catnip_lua_cursor->subscriptions,
    event->state == WLR_BUTTON_PRESSED ? "button::press" : "button:release",
    1
  );

  lua_pop(L, 1);
}
