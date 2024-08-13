#include "lua_cursor.h"
#include "desktop/cursor.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

struct catnip_lua_resource* catnip_lua_cursor = NULL;

static bool
catnip_lua_cursor__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  if (streq(key, "x")) {
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
    return false;
  }

  return true;
}

static bool
catnip_lua_cursor__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
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
  } else {
    return false;
  }

  return true;
}

void
catnip_lua_cursor_init(lua_State* L)
{
  catnip_lua_cursor = catnip_lua_resource_create(L);
  catnip_lua_cursor->data = catnip_cursor;
  catnip_lua_cursor->name = "cursor";
  catnip_lua_cursor->__index = catnip_lua_cursor__index;
  catnip_lua_cursor->__newindex = catnip_lua_cursor__newindex;
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
