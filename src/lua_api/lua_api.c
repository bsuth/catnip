#include "lua_api.h"
#include "lib_init.h"
#include "lib_keybindings.h"

void
init_lua_api(lua_State* L)
{
  load_lib_init(L);
  load_lib_keybindings(L);
}
