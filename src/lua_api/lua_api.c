#include "lua_api.h"
#include "keybindings.h"
#include <lua.h>

void
init_lua_api(lua_State* L)
{
  load_lib_keybindings(L);
}
