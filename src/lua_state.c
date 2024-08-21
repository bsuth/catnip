#include "lua_state.h"
#include "canvas/lua_canvas.h"
#include "desktop/lua_cursor.h"
#include "desktop/lua_key_event.h"
#include "desktop/lua_keyboard.h"
#include "desktop/lua_keyboards.h"
#include "desktop/lua_output.h"
#include "desktop/lua_output_mode.h"
#include "desktop/lua_output_modes.h"
#include "desktop/lua_outputs.h"
#include "desktop/lua_window.h"
#include "desktop/lua_windows.h"
#include "lua_catnip.h"
#include "lua_events.h"
#include "lua_keybindings.h"
#include <lauxlib.h>

void
catnip_lua_state_init(lua_State* L)
{
  catnip_lua_catnip_init(L);
  catnip_lua_events_init(L);
  catnip_lua_keybindings_init(L);

  catnip_lua_cursor_init(L);
  catnip_lua_key_event_init(L);
  catnip_lua_keyboard_init(L);
  catnip_lua_keyboards_init(L);
  catnip_lua_output_mode_init(L);
  catnip_lua_output_modes_init(L);
  catnip_lua_output_init(L);
  catnip_lua_outputs_init(L);
  catnip_lua_window_init(L);
  catnip_lua_windows_init(L);
  catnip_lua_canvas_init(L);

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip");

  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);
}

void
catnip_lua_state_populate(lua_State* L)
{
  catnip_lua_keyboards_populate(L);
  catnip_lua_outputs_populate(L);
  catnip_lua_windows_populate(L);
}
