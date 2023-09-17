#include "lua_api.h"
#include "lib_init.h"
#include "lib_keybindings.h"
#include "lua_api/windows.h"

void
init_lua_api(lua_State* L)
{
  init_lua_windows(L);
  load_lib_init(L);
}
