#include "lua_key_event.h"
#include "keyboard/keyboard.h"
#include "lua_event.h"
#include "utils/string.h"
#include <lauxlib.h>

static bool
lua_catnip_key_event__index(
  lua_State* L,
  struct catnip_lua_event* lua_event,
  const char* key
)
{
  struct catnip_key_event* key_event = lua_event->data;

  if (streq(key, "code")) {
    lua_pushnumber(L, key_event->xkb_keysym);
  } else if (streq(key, "name")) {
    lua_pushstring(L, key_event->xkb_name);
  } else if (streq(key, "shift")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_SHIFT);
  } else if (streq(key, "ctrl")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_CTRL);
  } else if (streq(key, "mod1")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_ALT);
  } else if (streq(key, "mod2")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_MOD2);
  } else if (streq(key, "mod3")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_MOD3);
  } else if (streq(key, "mod4")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_LOGO);
  } else if (streq(key, "mod5")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_MOD5);
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_key_event__newindex(
  lua_State* L,
  struct catnip_lua_event* lua_event,
  const char* key
)
{
  struct catnip_key_event* key_event = lua_event->data;

  if (streq(key, "prevent_notify")) {
    key_event->prevent_notify = lua_toboolean(L, 3);
  } else {
    return false;
  }

  return true;
}

void
lua_catnip_publish_key_event(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_key_event* event
)
{
  struct catnip_lua_event* lua_event = lua_catnip_event_create(L);

  lua_event->data = event;
  lua_event->__index = lua_catnip_key_event__index;
  lua_event->__newindex = lua_catnip_key_event__newindex;

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_event->ref);

  event->state == WL_KEYBOARD_KEY_STATE_PRESSED
    ? lua_catnip_resource_publish(L, keyboard->lua_resource, "key::press", 1)
    : lua_catnip_resource_publish(L, keyboard->lua_resource, "key::release", 1);

  lua_pop(L, 1);

  lua_catnip_event_destroy(L, lua_event);
}
