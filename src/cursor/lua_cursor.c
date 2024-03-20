#include "lua_cursor.h"
#include "cursor/cursor.h"
#include "cursor/cursor_properties.h"
#include "lua_events.h"
#include "utils/string.h"
#include <lauxlib.h>

struct catnip_lua_resource* lua_catnip_cursor = NULL;

static bool
lua_catnip_cursor__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  if (streq(key, "x")) {
    lua_pushnumber(L, catnip_cursor_get_x());
  } else if (streq(key, "y")) {
    lua_pushnumber(L, catnip_cursor_get_y());
  } else if (streq(key, "name")) {
    lua_pushstring(L, catnip_cursor_get_name());
  } else if (streq(key, "size")) {
    lua_pushnumber(L, catnip_cursor_get_size());
  } else if (streq(key, "theme")) {
    lua_pushstring(L, catnip_cursor_get_theme());
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_cursor__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  if (streq(key, "x")) {
    catnip_cursor_set_x(luaL_checknumber(L, 3));
  } else if (streq(key, "y")) {
    catnip_cursor_set_y(luaL_checknumber(L, 3));
  } else if (streq(key, "name")) {
    catnip_cursor_set_name(luaL_checkstring(L, 3));
  } else if (streq(key, "size")) {
    catnip_cursor_set_size(luaL_checknumber(L, 3));
  } else if (streq(key, "theme")) {
    catnip_cursor_set_theme(luaL_checkstring(L, 3));
  } else {
    return false;
  }

  return true;
}

void
lua_catnip_cursor_publish_button_event(
  lua_State* L,
  struct wlr_pointer_button_event* event
)
{
  lua_pushnumber(L, event->button);

  lua_catnip_events_publish(
    L,
    lua_catnip_cursor->subscriptions,
    event->state == WLR_BUTTON_PRESSED ? "button::press" : "button:release",
    1
  );

  lua_pop(L, 1);
}

void
lua_catnip_cursor_init(lua_State* L)
{
  lua_catnip_cursor = lua_catnip_resource_create(L);
  lua_catnip_cursor->data = catnip_cursor;
  lua_catnip_cursor->name = "cursor";
  lua_catnip_cursor->__index = lua_catnip_cursor__index;
  lua_catnip_cursor->__newindex = lua_catnip_cursor__newindex;
}
