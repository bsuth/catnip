#include "lua_api.h"
#include "server/lua_api/cursor.h"
#include "server/lua_api/lib_init.h"
#include "server/lua_api/lib_keybindings.h"
#include "server/lua_api/monitor.h"
#include "server/lua_api/windows.h"

void
init_lua_api(lua_State* L)
{
  load_lib_init(L);
  init_lua_windows(L);
  init_lua_monitors(L);
  init_lua_cursor(L);
}
