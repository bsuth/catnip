#include "lua_keyboard.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>

struct _catnip_lua_keyboard {
  lua_Ref ref;
  catnip_keyboard_t* keyboard;
  lua_Ref subscriptions;
  bool destroyed;
};

// -----------------------------------------------------------------------------
// Pubsub
// -----------------------------------------------------------------------------

static int
catnip_lua_keyboard_subscribe(lua_State* L)
{
  struct _catnip_lua_keyboard* lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, lua_keyboard->subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
catnip_lua_keyboard_unsubscribe(lua_State* L)
{
  struct _catnip_lua_keyboard* lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, lua_keyboard->subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
catnip_lua_keyboard_publish(lua_State* L)
{
  struct _catnip_lua_keyboard* lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");

  const char* event = luaL_checkstring(L, 2);

  lua_catnip_events_publish(
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
  struct _catnip_lua_keyboard* lua_keyboard = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (lua_keyboard->destroyed) {
    lua_log_error(L, "attempt to index outdated userdata");
    lua_pushnil(L);
  } else if (streq(key, "id")) {
    catnip_lua_keyboard_push_id(L, lua_keyboard);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, catnip_lua_keyboard_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, catnip_lua_keyboard_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, catnip_lua_keyboard_publish);
  } else if (streq(key, "name")) {
    catnip_lua_keyboard_push_name(L, lua_keyboard);
  } else if (streq(key, "xkb_rules")) {
    catnip_lua_keyboard_push_xkb_rules(L, lua_keyboard);
  } else if (streq(key, "xkb_model")) {
    catnip_lua_keyboard_push_xkb_model(L, lua_keyboard);
  } else if (streq(key, "xkb_layout")) {
    catnip_lua_keyboard_push_xkb_layout(L, lua_keyboard);
  } else if (streq(key, "xkb_variant")) {
    catnip_lua_keyboard_push_xkb_variant(L, lua_keyboard);
  } else if (streq(key, "xkb_options")) {
    catnip_lua_keyboard_push_xkb_options(L, lua_keyboard);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_keyboard__newindex(lua_State* L)
{
  struct _catnip_lua_keyboard* lua_keyboard = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (lua_keyboard->destroyed) {
    lua_log_error(L, "attempt to index outdated userdata");
  } else if (key == NULL) {
    lua_log_error(L, "attempt to index userdata with invalid key");
  } else if (streq(key, "xkb_rules")) {
    catnip_lua_keyboard_set_xkb_rules(L, lua_keyboard);
  } else if (streq(key, "xkb_model")) {
    catnip_lua_keyboard_set_xkb_model(L, lua_keyboard);
  } else if (streq(key, "xkb_layout")) {
    catnip_lua_keyboard_set_xkb_layout(L, lua_keyboard);
  } else if (streq(key, "xkb_variant")) {
    catnip_lua_keyboard_set_xkb_variant(L, lua_keyboard);
  } else if (streq(key, "xkb_options")) {
    catnip_lua_keyboard_set_xkb_options(L, lua_keyboard);
  } else {
    lua_log_warning(L, "attempt to index userdata with unknown key");
  }

  return 0;
}

// TODO: register metatable
static const struct luaL_Reg lua_catnip_keyboard_mt[] = {
  {"__index", catnip_lua_keyboard__index},
  {"__newindex", catnip_lua_keyboard__newindex},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct _catnip_lua_keyboard*
catnip_lua_keyboard_create(lua_State* L, catnip_keyboard_t* keyboard)
{
  struct _catnip_lua_keyboard* lua_keyboard =
    lua_newuserdata(L, sizeof(struct _catnip_lua_keyboard));
  luaL_setmetatable(L, "catnip.keyboard");

  lua_keyboard->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_keyboard->keyboard = keyboard;
  lua_keyboard->destroyed = false;

  lua_newtable(L);
  lua_keyboard->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  // TODO add keyboard listeners (destroy, key) + cleanup (__gc)
  // TODO create event (parent)

  return lua_keyboard;
}

void
catnip_lua_keyboard_destroy(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_catnip_events_publish(L, lua_keyboard->subscriptions, "destroy", 0);

  luaL_unref(L, LUA_REGISTRYINDEX, lua_keyboard->ref);
  luaL_unref(L, LUA_REGISTRYINDEX, lua_keyboard->subscriptions);

  lua_keyboard->destroyed = true;
}

lua_Ref
catnip_lua_keyboard_get_ref(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  return lua_keyboard->ref;
}

// -----------------------------------------------------------------------------
// Push
// -----------------------------------------------------------------------------

void
catnip_lua_keyboard_push_id(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushnumber(L, catnip_keyboard_get_id(lua_keyboard->keyboard));
}

void
catnip_lua_keyboard_push_name(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushstring(L, catnip_keyboard_get_name(lua_keyboard->keyboard));
}

void
catnip_lua_keyboard_push_xkb_rules(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushstring(L, catnip_keyboard_get_xkb_rules(lua_keyboard->keyboard));
}

void
catnip_lua_keyboard_push_xkb_model(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushstring(L, catnip_keyboard_get_xkb_model(lua_keyboard->keyboard));
}

void
catnip_lua_keyboard_push_xkb_layout(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushstring(L, catnip_keyboard_get_xkb_layout(lua_keyboard->keyboard));
}

void
catnip_lua_keyboard_push_xkb_variant(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushstring(L, catnip_keyboard_get_xkb_variant(lua_keyboard->keyboard));
}

void
catnip_lua_keyboard_push_xkb_options(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  lua_pushstring(L, catnip_keyboard_get_xkb_options(lua_keyboard->keyboard));
}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

void
catnip_lua_keyboard_set_xkb_rules(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  catnip_keyboard_set_xkb_rules(
    lua_keyboard->keyboard,
    lua_type(L, -1) == LUA_TNIL ? NULL : luaL_checkstring(L, 3)
  );
}

void
catnip_lua_keyboard_set_xkb_model(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  catnip_keyboard_set_xkb_model(
    lua_keyboard->keyboard,
    lua_type(L, -1) == LUA_TNIL ? NULL : luaL_checkstring(L, 3)
  );
}

void
catnip_lua_keyboard_set_xkb_layout(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  catnip_keyboard_set_xkb_layout(
    lua_keyboard->keyboard,
    lua_type(L, -1) == LUA_TNIL ? NULL : luaL_checkstring(L, 3)
  );
}

void
catnip_lua_keyboard_set_xkb_variant(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  catnip_keyboard_set_xkb_variant(
    lua_keyboard->keyboard,
    lua_type(L, -1) == LUA_TNIL ? NULL : luaL_checkstring(L, 3)
  );
}

void
catnip_lua_keyboard_set_xkb_options(
  lua_State* L,
  struct _catnip_lua_keyboard* lua_keyboard
)
{
  catnip_keyboard_set_xkb_options(
    lua_keyboard->keyboard,
    lua_type(L, -1) == LUA_TNIL ? NULL : luaL_checkstring(L, 3)
  );
}
