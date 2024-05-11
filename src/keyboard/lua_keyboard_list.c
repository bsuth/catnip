#include "lua_keyboard_list.h"
#include "keyboard/lua_keyboard.h"
#include "lua_resource_list.h"
#include <lauxlib.h>

struct catnip_lua_resource_list* lua_catnip_keyboard_list = NULL;

void
lua_catnip_keyboard_list_init(lua_State* L)
{
  lua_catnip_keyboard_list = lua_catnip_resource_list_create(L);
}

void
lua_catnip_keyboard_list_populate(lua_State* L)
{
  struct catnip_keyboard* keyboard = NULL;
  wl_list_for_each(keyboard, &catnip_keyboards, link)
  {
    lua_catnip_keyboard_create(L, keyboard);
  }
}
