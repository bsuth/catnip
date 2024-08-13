#include "lua_state.h"
#include "desktop/lua_cursor.h"
#include "desktop/lua_key_event.h"
#include "desktop/lua_keyboards.h"
#include "desktop/lua_outputs.h"
#include "desktop/lua_windows.h"
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
  catnip_lua_keyboards_init(L);
  catnip_lua_outputs_init(L);
  catnip_lua_windows_init(L);

  catnip_lua_events_init(L);

  catnip_lua_resource_init(L);
  catnip_lua_resource_list_init(L);
  catnip_lua_cursor_init(L); // must init after resource

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
