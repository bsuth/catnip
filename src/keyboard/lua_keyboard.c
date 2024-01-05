#include "lua_keyboard.h"
#include "events/lua_events.h"
#include "keyboard/lua_keyboard_key_event.h"
#include "keyboard/lua_keyboard_methods.h"
#include <glib.h>
#include <lauxlib.h>

lua_Ref lua_catnip_keyboards = LUA_NOREF;

static int
lua_catnip_keyboard__index(lua_State* L)
{
  struct catnip_keyboard** lua_keyboard = lua_touserdata(L, 1);
  struct catnip_keyboard* keyboard = *lua_keyboard;

  if (keyboard == NULL) {
    lua_log(L, "attempt to index outdated keyboard");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "id")) {
    lua_pushnumber(L, keyboard->id);
  } else if (g_str_equal(key, "name")) {
    lua_pushstring(L, keyboard->wlr_keyboard->base.name);
  } else if (g_str_equal(key, "xkb_rules")) {
    keyboard->xkb_rule_names.rules == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.rules);
  } else if (g_str_equal(key, "xkb_model")) {
    keyboard->xkb_rule_names.model == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.model);
  } else if (g_str_equal(key, "xkb_layout")) {
    keyboard->xkb_rule_names.layout == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.layout);
  } else if (g_str_equal(key, "xkb_variant")) {
    keyboard->xkb_rule_names.variant == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.variant);
  } else if (g_str_equal(key, "xkb_options")) {
    keyboard->xkb_rule_names.options == NULL
      ? lua_pushnil(L)
      : lua_pushstring(L, keyboard->xkb_rule_names.options);
  } else if (g_str_equal(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_keyboard_method_subscribe);
  } else if (g_str_equal(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_keyboard_method_unsubscribe);
  } else if (g_str_equal(key, "publish")) {
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
    lua_log(L, "attempt to index outdated keyboard");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_log(L, "invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "xkb_rules")) {
    keyboard->xkb_rule_names.rules = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (g_str_equal(key, "xkb_model")) {
    keyboard->xkb_rule_names.model = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (g_str_equal(key, "xkb_layout")) {
    keyboard->xkb_rule_names.layout = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (g_str_equal(key, "xkb_variant")) {
    keyboard->xkb_rule_names.variant = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else if (g_str_equal(key, "xkb_options")) {
    keyboard->xkb_rule_names.options = luaL_checkstring(L, 3);
    catnip_keyboard_reload_keymap(keyboard);
  } else {
    lua_log(L, "invalid key: %s", key);
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

  gchar* global_event = g_strconcat("keyboard::", event, NULL);
  lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard->lua.ref);
  lua_insert(L, -1 - nargs);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    global_event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
  g_free(global_event);
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

  if (event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    lua_catnip_keyboard_publish(L, keyboard, "keydown", 1);
  } else {
    lua_catnip_keyboard_publish(L, keyboard, "keyup", 1);
  }

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
