#include "lua_keyboard.h"
#include "desktop/lua_keyboards.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

static bool
catnip_lua_keyboard__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_keyboard* keyboard = lua_resource->data;

  if (streq(key, "id")) {
    lua_pushnumber(L, keyboard->id);
  } else if (streq(key, "name")) {
    lua_pushstring(L, keyboard->wlr.keyboard->base.name);
  } else if (streq(key, "xkb_rules")) {
    lua_pushstring(L, keyboard->xkb_rule_names.rules);
  } else if (streq(key, "xkb_model")) {
    lua_pushstring(L, keyboard->xkb_rule_names.model);
  } else if (streq(key, "xkb_layout")) {
    lua_pushstring(L, keyboard->xkb_rule_names.layout);
  } else if (streq(key, "xkb_variant")) {
    lua_pushstring(L, keyboard->xkb_rule_names.variant);
  } else if (streq(key, "xkb_options")) {
    lua_pushstring(L, keyboard->xkb_rule_names.options);
  } else {
    return false;
  }

  return true;
}

static bool
catnip_lua_keyboard__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_keyboard* keyboard = lua_resource->data;

  if (streq(key, "xkb_rules")) {
    catnip_keyboard_update_xkb_rule_name(
      keyboard,
      &keyboard->xkb_rule_names.rules,
      lua_type(L, 3) == LUA_TNIL ? NULL : strdup(luaL_checkstring(L, 3))
    );
  } else if (streq(key, "xkb_model")) {
    catnip_keyboard_update_xkb_rule_name(
      keyboard,
      &keyboard->xkb_rule_names.model,
      lua_type(L, 3) == LUA_TNIL ? NULL : strdup(luaL_checkstring(L, 3))
    );
  } else if (streq(key, "xkb_layout")) {
    catnip_keyboard_update_xkb_rule_name(
      keyboard,
      &keyboard->xkb_rule_names.layout,
      lua_type(L, 3) == LUA_TNIL ? NULL : strdup(luaL_checkstring(L, 3))
    );
  } else if (streq(key, "xkb_variant")) {
    catnip_keyboard_update_xkb_rule_name(
      keyboard,
      &keyboard->xkb_rule_names.variant,
      lua_type(L, 3) == LUA_TNIL ? NULL : strdup(luaL_checkstring(L, 3))
    );
  } else if (streq(key, "xkb_options")) {
    catnip_keyboard_update_xkb_rule_name(
      keyboard,
      &keyboard->xkb_rule_names.options,
      lua_type(L, 3) == LUA_TNIL ? NULL : strdup(luaL_checkstring(L, 3))
    );
  } else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct catnip_lua_resource*
catnip_lua_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard)
{
  struct catnip_lua_resource* lua_resource = catnip_lua_resource_create(L);
  keyboard->lua_resource = lua_resource;

  lua_resource->data = keyboard;
  lua_resource->name = "keyboard";
  lua_resource->__index = catnip_lua_keyboard__index;
  lua_resource->__newindex = catnip_lua_keyboard__newindex;

  wl_list_insert(&catnip_lua_keyboards->keyboards, &lua_resource->link);

  catnip_lua_keyboard_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
catnip_lua_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  catnip_lua_keyboard_publish(L, lua_resource, "destroy", 0);
  catnip_lua_resource_destroy(L, lua_resource);
}

void
catnip_lua_keyboard_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
)
{
  catnip_lua_resource_publish(L, lua_resource, event, nargs);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
  lua_insert(L, -1 - nargs);

  catnip_lua_events_publish(
    L,
    catnip_lua_keyboards->subscriptions,
    event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
}
