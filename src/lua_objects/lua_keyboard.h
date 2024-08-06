#ifndef CATNIP_LUA_OBJECTS_LUA_KEYBOARD_H
#define CATNIP_LUA_OBJECTS_LUA_KEYBOARD_H

#include "extensions/lua.h"
#include "objects/keyboard.h"

typedef struct _catnip_lua_keyboard catnip_lua_keyboard_t;

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_lua_keyboard_t*
catnip_lua_keyboard_create(lua_State* L, catnip_keyboard_t* keyboard);

void
catnip_lua_keyboard_destroy(lua_State* L, catnip_lua_keyboard_t* lua_keyboard);

lua_Ref
catnip_lua_keyboard_get_ref(lua_State* L, catnip_lua_keyboard_t* lua_keyboard);

// -----------------------------------------------------------------------------
// Push
// -----------------------------------------------------------------------------

void
catnip_lua_keyboard_push_id(lua_State* L, catnip_lua_keyboard_t* lua_keyboard);

void
catnip_lua_keyboard_push_name(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_push_xkb_rules(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_push_xkb_model(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_push_xkb_layout(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_push_xkb_variant(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_push_xkb_options(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

void
catnip_lua_keyboard_set_xkb_rules(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_set_xkb_model(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_set_xkb_layout(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_set_xkb_variant(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

void
catnip_lua_keyboard_set_xkb_options(
  lua_State* L,
  catnip_lua_keyboard_t* lua_keyboard
);

#endif
