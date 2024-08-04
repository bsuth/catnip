#include "lua_keyboard.h"
#include "extensions/string.h"
#include "keyboard/lua_keyboard_list.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

static bool
lua_catnip_keyboard__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_keyboard* keyboard = lua_resource->data;

  if (streq(key, "id")) {
    lua_pushnumber(L, keyboard->id);
  } else if (streq(key, "name")) {
    lua_pushstring(L, keyboard->wlr_keyboard->base.name);
  } else if (streq(key, "xkb_rules")) {
    keyboard->xkb_rules == NULL ? lua_pushnil(L)
                                : lua_pushstring(L, keyboard->xkb_rules);
  } else if (streq(key, "xkb_model")) {
    keyboard->xkb_model == NULL ? lua_pushnil(L)
                                : lua_pushstring(L, keyboard->xkb_model);
  } else if (streq(key, "xkb_layout")) {
    keyboard->xkb_layout == NULL ? lua_pushnil(L)
                                 : lua_pushstring(L, keyboard->xkb_layout);
  } else if (streq(key, "xkb_variant")) {
    keyboard->xkb_variant == NULL ? lua_pushnil(L)
                                  : lua_pushstring(L, keyboard->xkb_variant);
  } else if (streq(key, "xkb_options")) {
    keyboard->xkb_options == NULL ? lua_pushnil(L)
                                  : lua_pushstring(L, keyboard->xkb_options);
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

  char** xkb_pointer = NULL;

  if (streq(key, "xkb_rules")) {
    xkb_pointer = &keyboard->xkb_rules;
  } else if (streq(key, "xkb_model")) {
    xkb_pointer = &keyboard->xkb_model;
  } else if (streq(key, "xkb_layout")) {
    xkb_pointer = &keyboard->xkb_layout;
  } else if (streq(key, "xkb_variant")) {
    xkb_pointer = &keyboard->xkb_variant;
  } else if (streq(key, "xkb_options")) {
    xkb_pointer = &keyboard->xkb_options;
  } else {
    return false;
  }

  if (xkb_pointer != NULL) {
    char* prev = *xkb_pointer;

    *xkb_pointer =
      lua_type(L, 3) == LUA_TNIL ? NULL : strdup(luaL_checkstring(L, 3));

    if (prev != NULL) {
      free(prev);
    }

    catnip_keyboard_reload_keymap(keyboard);
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
