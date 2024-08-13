#include "lua_state.h"
#include "desktop/lua_cursor.h"
#include "desktop/lua_key_event.h"
#include "desktop/lua_keyboard_list.h"
#include "desktop/lua_output_list.h"
#include "desktop/lua_window_list.h"
#include "lua_catnip.h"
#include "lua_events.h"
#include "lua_resource.h"
#include "lua_resource_list.h"
#include <lauxlib.h>

void
catnip_lua_state_init(lua_State* L)
{
  catnip_lua_catnip_init(L);

  catnip_lua_key_event_init(L);
  catnip_lua_events_init(L);

  catnip_lua_resource_init(L);
  catnip_lua_resource_list_init(L);

  catnip_lua_cursor_init(L); // must init after resource
  catnip_lua_keyboard_list_init(L); // must init after resource
  catnip_lua_output_list_init(L); // must init after resource
  catnip_lua_window_list_init(L); // must init after resource

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
  catnip_lua_keyboard_list_populate(L);
  catnip_lua_output_list_populate(L);
  catnip_lua_window_list_populate(L);
}
