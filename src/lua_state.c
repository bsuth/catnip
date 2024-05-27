#include "lua_state.h"
#include "canvas/lua_png.h"
#include "canvas/lua_svg.h"
#include "cursor/lua_cursor.h"
#include "keyboard/lua_keyboard_list.h"
#include "lua_catnip.h"
#include "lua_event.h"
#include "lua_events.h"
#include "lua_resource.h"
#include "lua_resource_list.h"
#include "output/lua_output_list.h"
#include "window/lua_window_list.h"
#include <lauxlib.h>

void
lua_catnip_state_init(lua_State* L)
{
  lua_catnip_init(L);

  lua_catnip_event_init(L);
  lua_catnip_events_init(L);

  lua_catnip_png_init(L);
  lua_catnip_svg_init(L);

  lua_catnip_resource_init(L);
  lua_catnip_resource_list_init(L);

  lua_catnip_cursor_init(L); // must init after resource
  lua_catnip_keyboard_list_init(L); // must init after resource
  lua_catnip_output_list_init(L); // must init after resource
  lua_catnip_window_list_init(L); // must init after resource
                                  //
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip");

  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);
}

void
lua_catnip_state_populate(lua_State* L)
{
  lua_catnip_keyboard_list_populate(L);
  lua_catnip_output_list_populate(L);
  lua_catnip_window_list_populate(L);
}
