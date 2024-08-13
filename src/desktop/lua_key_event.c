#include "lua_key_event.h"
#include "desktop/keyboard.h"
#include "extensions/string.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
lua_catnip_key_event__index(lua_State* L)
{
  struct lua_catnip_key_event* lua_key_event = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (lua_key_event == NULL) {
    lua_log_error(L, "attempt to index outdated key event");
    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L); // cannot conver to string
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
  } else {
    lua_pushnil(L);
  }

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static int
lua_catnip_key_event__newindex(lua_State* L)
{
  struct lua_catnip_key_event* lua_key_event = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (streq(key, "prevent_notify")) {
    lua_key_event->key_event->prevent_notify = lua_toboolean(L, 3);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg lua_catnip_key_event_mt[] = {
  {"__index", lua_catnip_key_event__index},
  {"__newindex", lua_catnip_key_event__newindex},
  {NULL, NULL}
};

void
lua_catnip_key_event_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.key.event");
  luaL_setfuncs(L, lua_catnip_key_event_mt, 0);
  lua_pop(L, 1);
}

void
lua_catnip_publish_key_event(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_key_event* key_event
)
{
  struct lua_catnip_key_event* lua_key_event =
    lua_newuserdata(L, sizeof(struct lua_catnip_key_event));
  luaL_setmetatable(L, "catnip.key.event");

  lua_key_event->key_event = key_event;

  key_event->state == WL_KEYBOARD_KEY_STATE_PRESSED
    ? lua_catnip_resource_publish(L, keyboard->lua_resource, "keypress", 1)
    : lua_catnip_resource_publish(L, keyboard->lua_resource, "keyrelease", 1);

  lua_key_event->key_event = NULL;
  lua_pop(L, 1);
}