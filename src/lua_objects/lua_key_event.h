#ifndef CATNIP_LUA_OBJECTS_LUA_KEY_EVENT_H
#define CATNIP_LUA_OBJECTS_LUA_KEY_EVENT_H

#include "extensions/lua.h"
#include "objects/key_event.h"

typedef struct _catnip_lua_key_event catnip_lua_key_event_t;

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_lua_key_event_t*
catnip_lua_key_event_create(lua_State* L, catnip_key_event_t* key_event);

void
catnip_lua_key_event_destroy(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

lua_Ref
catnip_lua_key_event_get_ref(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

// -----------------------------------------------------------------------------
// Push
// -----------------------------------------------------------------------------

static void
catnip_lua_key_event_push_code(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_char(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_name(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_shift(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_ctrl(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_mod1(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_mod2(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_mod3(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_mod4(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

static void
catnip_lua_key_event_push_mod5(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

static void
catnip_lua_key_event_set_prevent_notify(
  lua_State* L,
  catnip_lua_key_event_t* lua_key_event
);

#endif
