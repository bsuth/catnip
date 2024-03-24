#include "lua_cursor.h"
#include "cursor/cursor.h"
#include "cursor/cursor_methods.h"
#include "cursor/lua_cursor_methods.h"
#include "lua.h"
#include "lua_events.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

struct catnip_lua_resource* lua_catnip_cursor = NULL;

static bool
lua_catnip_cursor__index(
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
    lua_pushstring(L, catnip_cursor_name);
  } else if (streq(key, "size")) {
    lua_pushnumber(L, catnip_xcursor_manager->size);
  } else if (streq(key, "theme")) {
    lua_pushstring(L, catnip_xcursor_manager->name);
  } else if (streq(key, "move")) {
    lua_pushcfunction(L, lua_catnip_cursor_method_move);
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
    catnip_cursor_move(luaL_checknumber(L, 3), catnip_cursor->y);
  } else if (streq(key, "y")) {
    catnip_cursor_move(catnip_cursor->x, luaL_checknumber(L, 3));
  } else if (streq(key, "name")) {
    const char* new_name = luaL_checkstring(L, 3);

    if (catnip_cursor_name != NULL) {
      free(catnip_cursor_name);
      catnip_cursor_name = NULL;
    }

    if (new_name == NULL) {
      wlr_cursor_unset_image(catnip_cursor);
    } else {
      catnip_cursor_name = strdup(new_name);
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
