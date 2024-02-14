#include "lua_cursor.h"
#include "cursor/cursor_properties.h"
#include "cursor/lua_cursor_methods.h"
#include "events/lua_events.h"
#include <glib.h>
#include <lauxlib.h>

lua_Ref lua_catnip_cursor = LUA_NOREF;
lua_Ref lua_catnip_cursor_subscriptions = LUA_NOREF;

static int
lua_catnip_cursor__index(lua_State* L)
{
  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, catnip_cursor_get_x());
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, catnip_cursor_get_y());
  } else if (g_str_equal(key, "name")) {
    lua_pushstring(L, catnip_cursor_get_name());
  } else if (g_str_equal(key, "size")) {
    lua_pushnumber(L, catnip_cursor_get_size());
  } else if (g_str_equal(key, "theme")) {
    lua_pushstring(L, catnip_cursor_get_theme());
  } else if (g_str_equal(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_cursor_method_subscribe);
  } else if (g_str_equal(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_cursor_method_unsubscribe);
  } else if (g_str_equal(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_cursor_method_publish);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_cursor__newindex(lua_State* L)
{
  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    catnip_cursor_set_x(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    catnip_cursor_set_y(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "name")) {
    catnip_cursor_set_name(luaL_checkstring(L, 3));
  } else if (g_str_equal(key, "size")) {
    catnip_cursor_set_size(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "theme")) {
    catnip_cursor_set_theme(luaL_checkstring(L, 3));
  } else {
    lua_log_error(L, "unknown userdata field (%s)", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_cursor_mt[] = {
  {"__index", lua_catnip_cursor__index},
  {"__newindex", lua_catnip_cursor__newindex},
  {NULL, NULL}
};

void
lua_catnip_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
)
{
  lua_pushnumber(L, event->button);

  lua_catnip_events_publish(
    L,
    lua_catnip_cursor_subscriptions,
    event->state == WLR_BUTTON_PRESSED ? "button::press" : "button:release",
    1
  );

  lua_pop(L, 1);
}

void
lua_catnip_cursor_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.cursor");
  luaL_setfuncs(L, lua_catnip_cursor_mt, 0);
  lua_pop(L, 1);

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.cursor");
  lua_catnip_cursor = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  lua_catnip_cursor_subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}
