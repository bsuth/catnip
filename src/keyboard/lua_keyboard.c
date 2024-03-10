#include "lua_keyboard.h"
#include "keyboard/keyboard_methods.h"
#include "keyboard/lua_keyboard_key_event.h"
#include "keyboard/lua_keyboard_methods.h"
#include "lua_events.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>

lua_Ref lua_catnip_keyboards = LUA_NOREF;

static int
lua_catnip_keyboard__index(lua_State* L)
{
  struct catnip_keyboard** lua_keyboard = lua_touserdata(L, 1);
  struct catnip_keyboard* keyboard = *lua_keyboard;

  if (keyboard == NULL) {
    lua_log_error(L, "attempt to index outdated userdata");
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "id")) {
    lua_pushnumber(L, keyboard->id);
  } else if (streq(key, "name")) {
    lua_pushstring(L, keyboard->wlr_keyboard->base.name);
  } else if (streq(key, "xkb_rules")) {
    keyboard->xkb_rule_names.rules == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.rules);
  } else if (streq(key, "xkb_model")) {
    keyboard->xkb_rule_names.model == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.model);
  } else if (streq(key, "xkb_layout")) {
    keyboard->xkb_rule_names.layout == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.layout);
  } else if (streq(key, "xkb_variant")) {
    keyboard->xkb_rule_names.variant == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.variant);
  } else if (streq(key, "xkb_options")) {
    keyboard->xkb_rule_names.options == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.options);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_keyboard_method_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_keyboard_method_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_keyboard_method_publish);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_keyboard__newindex(lua_State* L)
{
  struct catnip_keyboard** lua_keyboard = lua_touserdata(L, 1);
  struct catnip_keyboard* keyboard = *lua_keyboard;

  if (keyboard == NULL) {
    lua_log_error(L, "attempt to index outdated userdata");
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "xkb_rules")) {
    keyboard->xkb_rule_names.rules = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (streq(key, "xkb_model")) {
    keyboard->xkb_rule_names.model = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (streq(key, "xkb_layout")) {
    keyboard->xkb_rule_names.layout = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (streq(key, "xkb_variant")) {
    keyboard->xkb_rule_names.variant = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (streq(key, "xkb_options")) {
    keyboard->xkb_rule_names.options = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else {
    lua_log_error(L, "unknown userdata field (%s)", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_keyboard_mt[] = {
  {"__index", lua_catnip_keyboard__index},
  {"__newindex", lua_catnip_keyboard__newindex},
  {NULL, NULL}
};

void
lua_catnip_keyboard_destroy(lua_State* L, struct catnip_keyboard* keyboard)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_keyboards);
  lua_pushnil(L);
  lua_rawseti(L, -2, keyboard->id);
  lua_pop(L, 1);

  lua_catnip_keyboard_publish(L, keyboard, "destroy", 0);

  *(keyboard->lua.userdata) = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, keyboard->lua.ref);
  luaL_unref(L, LUA_REGISTRYINDEX, keyboard->lua.subscriptions);
}

void
lua_catnip_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_keyboards);

  struct catnip_keyboard** lua_keyboard =
    lua_newuserdata(L, sizeof(struct catnip_keyboard*));
  luaL_setmetatable(L, "catnip.keyboard");

  *lua_keyboard = keyboard;
  keyboard->lua.userdata = lua_keyboard;

  lua_pushvalue(L, -1);
  keyboard->lua.ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  keyboard->lua.subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawseti(L, -2, keyboard->id);
  lua_pop(L, 1);

  lua_catnip_keyboard_publish(L, keyboard, "create", 0);
}

void
lua_catnip_keyboard_publish(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  const char* event,
  int nargs
)
{
  lua_catnip_events_publish(L, keyboard->lua.subscriptions, event, nargs);

  char* global_event = strfmt("keyboard::%s", event);
  lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard->lua.ref);
  lua_insert(L, -1 - nargs);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    global_event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
  free(global_event);
}

void
lua_catnip_keyboard_publish_key_event(
  lua_State* L,
  struct catnip_keyboard* keyboard,
  struct catnip_keyboard_key_event* event
)
{
  struct catnip_keyboard_key_event** lua_event =
    lua_newuserdata(L, sizeof(struct catnip_keyboard_key_event*));
  luaL_setmetatable(L, "catnip.keyboard.key.event");

  *lua_event = event;

  event->state == WL_KEYBOARD_KEY_STATE_PRESSED
    ? lua_catnip_keyboard_publish(L, keyboard, "key::press", 1)
    : lua_catnip_keyboard_publish(L, keyboard, "key::release", 1);

  *lua_event = NULL;

  lua_pop(L, 1);
}

void
lua_catnip_keyboard_init(lua_State* L)
{
  lua_newtable(L);
  lua_catnip_keyboards = luaL_ref(L, LUA_REGISTRYINDEX);

  luaL_newmetatable(L, "catnip.keyboard");
  luaL_setfuncs(L, lua_catnip_keyboard_mt, 0);
  lua_pop(L, 1);

  lua_catnip_keyboard_key_event_init(L);

  struct catnip_keyboard* keyboard = NULL;
  wl_list_for_each(keyboard, &catnip_keyboards, link)
  {
    lua_catnip_keyboard_create(L, keyboard);
  }
}
