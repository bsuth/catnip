#include "lua_keyboards.h"
#include "extensions/wayland.h"
#include "lua_objects/lua_object_list.h"
#include "state/keyboards.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct catnip_lua_keyboards_state*
catnip_lua_keyboards_create(lua_State* L)
{
  catnip_lua_object_list_t* lua_keyboard = catnip_lua_object_list_create(L);

  wl_signal_subscribe(
    &catnip_keyboards_state.signals.create,
    &listeners.backend_new_input,
    on_new_input
  );
}
