#include "lua_key_event.h"
#include "desktop/keyboard.h"
#include "desktop/lua_keyboard.h"
#include "extensions/string.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_key_event__index(lua_State* L)
{
  struct catnip_lua_key_event* lua_key_event = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (lua_key_event->key_event == NULL) {
    lua_log_error(L, "attempt to index outdated key event");
    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "code")) {
    lua_pushnumber(L, lua_key_event->key_event->xkb_keysym);
  } else if (streq(key, "char")) {
    if (lua_key_event->key_event->xkb_keysym == 9) {
      lua_pushstring(L, "\t");
    } else if (lua_key_event->key_event->xkb_keysym == 10) {
      lua_pushstring(L, "\n");
    } else if (31 < lua_key_event->key_event->xkb_keysym
               && lua_key_event->key_event->xkb_keysym < 127) {
      lua_pushstring(L, (char*) &lua_key_event->key_event->xkb_keysym);
    } else {
      lua_pushnil(L);
    }
  } else if (streq(key, "name")) {
    char xkb_name[64];
    xkb_keysym_get_name(
      lua_key_event->key_event->xkb_keysym,
      xkb_name,
      sizeof(xkb_name)
    );
    lua_pushstring(L, xkb_name);
  } else if (streq(key, "shift")) {
    lua_pushboolean(
      L,
      lua_key_event->key_event->modifiers & WLR_MODIFIER_SHIFT
    );
  } else if (streq(key, "ctrl")) {
    lua_pushboolean(L, lua_key_event->key_event->modifiers & WLR_MODIFIER_CTRL);
  } else if (streq(key, "mod1")) {
    lua_pushboolean(L, lua_key_event->key_event->modifiers & WLR_MODIFIER_ALT);
  } else if (streq(key, "mod2")) {
    lua_pushboolean(L, lua_key_event->key_event->modifiers & WLR_MODIFIER_MOD2);
  } else if (streq(key, "mod3")) {
    lua_pushboolean(L, lua_key_event->key_event->modifiers & WLR_MODIFIER_MOD3);
  } else if (streq(key, "mod4")) {
    lua_pushboolean(L, lua_key_event->key_event->modifiers & WLR_MODIFIER_LOGO);
  } else if (streq(key, "mod5")) {
    lua_pushboolean(L, lua_key_event->key_event->modifiers & WLR_MODIFIER_MOD5);
  } else if (streq(key, "propagate")) {
    lua_pushboolean(L, lua_key_event->key_event->propagate);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static int
catnip_lua_key_event__newindex(lua_State* L)
{
  struct catnip_lua_key_event* lua_key_event = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  }

  if (lua_key_event->key_event == NULL) {
    lua_log_error(L, "attempt to index outdated key event");
  } else if (streq(key, "propagate")) {
    lua_key_event->key_event->propagate = lua_toboolean(L, 3);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_key_event_mt[] = {
  {"__index", catnip_lua_key_event__index},
  {"__newindex", catnip_lua_key_event__newindex},
  {NULL, NULL}
};

void
catnip_lua_key_event_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.key.event");
  luaL_setfuncs(L, catnip_lua_key_event_mt, 0);
  lua_pop(L, 1);
}

void
catnip_lua_key_event_publish(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_key_event* key_event
)
{
  struct catnip_lua_key_event* lua_key_event =
    lua_newuserdata(L, sizeof(struct catnip_lua_key_event));
  luaL_setmetatable(L, "catnip.key.event");

  lua_key_event->key_event = key_event;

  key_event->state == WL_KEYBOARD_KEY_STATE_PRESSED
    ? catnip_lua_keyboard_publish(L, keyboard->lua_keyboard, "keypress", 1)
    : catnip_lua_keyboard_publish(L, keyboard->lua_keyboard, "keyrelease", 1);

  // Explicitly set `NULL`, just in case the user is keeping a reference.
  lua_key_event->key_event = NULL;

  lua_pop(L, 1);
}
