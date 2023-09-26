#include "lua_api.h"
#include "lua_api/cursor.h"
#include "lua_api/lib_init.h"
#include "lua_api/lib_keybindings.h"
#include "lua_api/monitor.h"
#include "lua_api/windows.h"

void
init_lua_api(lua_State* L)
{
  load_lib_init(L);
  init_lua_windows(L);
  init_lua_monitors(L);
  init_lua_cursor(L);
}
