#include "lua_api.h"
#include "cursor.h"
#include "lib_init.h"
#include "lib_keybindings.h"
#include "monitor.h"
#include "windows.h"

void
init_lua_api(lua_State* L)
{
  load_lib_init(L);
  init_lua_windows(L);
  init_lua_monitors(L);
  init_lua_cursor(L);
}
