#include "lua_keyboard.h"
#include "desktop/lua_keyboards.h"
#include "extensions/string.h"
#include "log.h"
#include "lua_events.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Lua Methods
// -----------------------------------------------------------------------------

static int
catnip_lua_keyboard_lua_on(lua_State* L)
{
  struct catnip_lua_keyboard* lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, lua_keyboard->subscriptions, event);

  return 1;
}

static int
catnip_lua_keyboard_lua_emit(lua_State* L)
{
  struct catnip_lua_keyboard* lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    lua_keyboard->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_keyboard__index(lua_State* L)
{
  struct catnip_lua_keyboard* lua_keyboard = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  struct catnip_keyboard* keyboard = lua_keyboard->keyboard;

  if (keyboard == NULL) {
    catnip_log_error("attempt to index outdated keyboard");
    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "id")) {
    lua_pushnumber(L, keyboard->id);
  } else if (streq(key, "on")) {
    lua_pushcfunction(L, catnip_lua_keyboard_lua_on);
  } else if (streq(key, "emit")) {
    lua_pushcfunction(L, catnip_lua_keyboard_lua_emit);
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
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_keyboard__newindex(lua_State* L)
{
  struct catnip_lua_keyboard* lua_keyboard = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  struct catnip_keyboard* keyboard = lua_keyboard->keyboard;

  if (key == NULL) {
    return 0;
  }

  if (keyboard == NULL) {
    catnip_log_error("attempt to index outdated keyboard");
  } else if (streq(key, "xkb_rules")) {
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
  }

  return 0;
}

static const struct luaL_Reg catnip_lua_keyboard_mt[] = {
  {"__index", catnip_lua_keyboard__index},
  {"__newindex", catnip_lua_keyboard__newindex},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_keyboard_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.keyboard");
  luaL_setfuncs(L, catnip_lua_keyboard_mt, 0);
  lua_pop(L, 1);
}

void
catnip_lua_keyboard_create(lua_State* L, struct catnip_keyboard* keyboard)
{
  struct catnip_lua_keyboard* lua_keyboard =
    lua_newuserdata(L, sizeof(struct catnip_lua_keyboard));
  luaL_setmetatable(L, "catnip.keyboard");

  lua_keyboard->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_keyboard->keyboard = keyboard;

  lua_newtable(L);
  lua_keyboard->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_list_insert(&catnip_lua_keyboards->keyboards, &lua_keyboard->link);

  // Assign `keyboard->lua_keyboard` here, since the Lua keyboard may be created
  // much later than the actual keyboard (ex. on config reload).
  keyboard->lua_keyboard = lua_keyboard;

  catnip_lua_keyboard_publish(L, lua_keyboard, "create", 0);
}

void
catnip_lua_keyboard_destroy(
  lua_State* L,
  struct catnip_lua_keyboard* lua_keyboard
)
{
  catnip_lua_keyboard_publish(L, lua_keyboard, "destroy", 0);

  // Explicitly set `NULL`, just in case the user is keeping a reference.
  lua_keyboard->keyboard = NULL;

  luaL_unref(L, LUA_REGISTRYINDEX, lua_keyboard->subscriptions);
  luaL_unref(L, LUA_REGISTRYINDEX, lua_keyboard->ref);
  wl_list_remove(&lua_keyboard->link);
}

void
catnip_lua_keyboard_publish(
  lua_State* L,
  struct catnip_lua_keyboard* lua_keyboard,
  const char* event,
  int nargs
)
{
  catnip_lua_events_publish(L, lua_keyboard->subscriptions, event, nargs);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_keyboard->ref);
  lua_insert(L, -1 - nargs);

  catnip_lua_events_publish(
    L,
    catnip_lua_keyboards->subscriptions,
    event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
}
