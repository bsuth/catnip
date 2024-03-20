#include "lua_keyboard.h"
#include "keyboard/keyboard_methods.h"
#include "keyboard/lua_keyboard_list.h"
#include "lua_resource.h"
#include "utils/string.h"
#include <lauxlib.h>

static bool
lua_catnip_keyboard__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_keyboard* keyboard = lua_resource->data;

  if (streq(key, "name")) {
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
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_keyboard__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_keyboard* keyboard = lua_resource->data;

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
    return false;
  }

  return true;
}

struct catnip_lua_resource*
lua_catnip_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard)
{
  struct catnip_lua_resource* lua_resource = lua_catnip_resource_create(L);
  keyboard->lua_resource = lua_resource;

  lua_resource->data = keyboard;
  lua_resource->name = "keyboard";
  lua_resource->__index = lua_catnip_keyboard__index;
  lua_resource->__newindex = lua_catnip_keyboard__newindex;

  wl_list_insert(&lua_catnip_keyboard_list->head, &lua_resource->link);

  lua_catnip_resource_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
lua_catnip_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  lua_catnip_resource_publish(L, lua_resource, "destroy", 0);
  lua_catnip_resource_destroy(L, lua_resource);
}
