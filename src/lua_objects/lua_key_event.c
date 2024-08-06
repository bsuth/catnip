#include "lua_key_event.h"
#include "extensions/string.h"
#include <lauxlib.h>

struct _catnip_lua_key_event {
  lua_Ref ref;
  catnip_key_event_t* key_event;
  bool destroyed;
};

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_key_event__index(lua_State* L)
{
  struct _catnip_lua_key_event* lua_key_event = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (lua_key_event->destroyed) {
    lua_log_error(L, "attempt to index outdated userdata");
    lua_pushnil(L);
  } else if (streq(key, "code")) {
    catnip_lua_key_event_push_code(L, lua_key_event);
  } else if (streq(key, "char")) {
    catnip_lua_key_event_push_char(L, lua_key_event);
  } else if (streq(key, "name")) {
    catnip_lua_key_event_push_name(L, lua_key_event);
  } else if (streq(key, "shift")) {
    catnip_lua_key_event_push_shift(L, lua_key_event);
  } else if (streq(key, "ctrl")) {
    catnip_lua_key_event_push_ctrl(L, lua_key_event);
  } else if (streq(key, "mod1")) {
    catnip_lua_key_event_push_mod1(L, lua_key_event);
  } else if (streq(key, "mod2")) {
    catnip_lua_key_event_push_mod2(L, lua_key_event);
  } else if (streq(key, "mod3")) {
    catnip_lua_key_event_push_mod3(L, lua_key_event);
  } else if (streq(key, "mod4")) {
    catnip_lua_key_event_push_mod4(L, lua_key_event);
  } else if (streq(key, "mod5")) {
    catnip_lua_key_event_push_mod5(L, lua_key_event);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_key_event__newindex(lua_State* L)
{
  struct _catnip_lua_key_event* lua_key_event = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (lua_key_event->destroyed) {
    lua_log_error(L, "attempt to index outdated userdata");
  } else if (key == NULL) {
    lua_log_error(L, "attempt to index userdata with invalid key");
  } else if (streq(key, "prevent_notify")) {
    catnip_lua_key_event_set_prevent_notify(L, lua_key_event);
  } else {
    lua_log_warning(L, "attempt to index userdata with unknown key");
  }

  return 0;
}

// TODO: register metatable
static const struct luaL_Reg lua_catnip_key_event_mt[] = {
  {"__index", catnip_lua_key_event__index},
  {"__newindex", catnip_lua_key_event__newindex},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_lua_key_event_t*
catnip_lua_key_event_create(lua_State* L, catnip_key_event_t* key_event)
{
  struct _catnip_lua_key_event* lua_key_event =
    lua_newuserdata(L, sizeof(struct _catnip_lua_key_event));
  luaL_setmetatable(L, "catnip.key.event");

  lua_key_event->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_key_event->key_event = key_event;
  lua_key_event->destroyed = false;

  return lua_key_event;
}

void
catnip_lua_key_event_destroy(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  luaL_unref(L, LUA_REGISTRYINDEX, lua_key_event->ref);
  lua_key_event->destroyed = true;
}

lua_Ref
catnip_lua_key_event_get_ref(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  return lua_key_event->ref;
}

// -----------------------------------------------------------------------------
// Push
// -----------------------------------------------------------------------------

static void
catnip_lua_key_event_push_code(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  lua_pushnumber(L, catnip_key_event_get_xkb_keysym(lua_key_event->key_event));
}

static void
catnip_lua_key_event_push_char(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  xkb_keysym_t xkb_keysym =
    catnip_key_event_get_xkb_keysym(lua_key_event->key_event);

  if (xkb_keysym == 9) {
    lua_pushstring(L, "\t");
  } else if (xkb_keysym == 10) {
    lua_pushstring(L, "\n");
  } else if (31 < xkb_keysym && xkb_keysym < 127) {
    lua_pushstring(L, (char*) &xkb_keysym);
  } else {
    lua_pushnil(L);
  }
}

static void
catnip_lua_key_event_push_name(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  lua_pushstring(L, catnip_key_event_get_xkb_name(lua_key_event->key_event));
}

static void
catnip_lua_key_event_push_shift(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_SHIFT);
}

static void
catnip_lua_key_event_push_ctrl(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_CTRL);
}

static void
catnip_lua_key_event_push_mod1(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_ALT);
}

static void
catnip_lua_key_event_push_mod2(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_MOD2);
}

static void
catnip_lua_key_event_push_mod3(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_MOD3);
}

static void
catnip_lua_key_event_push_mod4(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_LOGO);
}

static void
catnip_lua_key_event_push_mod5(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  uint32_t modifiers = catnip_key_event_get_modifiers(lua_key_event->key_event);
  lua_pushboolean(L, modifiers & WLR_MODIFIER_MOD5);
}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

static void
catnip_lua_key_event_set_prevent_notify(
  lua_State* L,
  struct _catnip_lua_key_event* lua_key_event
)
{
  catnip_key_event_set_prevent_notify(
    lua_key_event->key_event,
    lua_toboolean(L, -1)
  );
}
