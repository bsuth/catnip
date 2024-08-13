#include "lua_keyboard_list.h"
#include "desktop/keyboards.h"
#include "desktop/lua_keyboard.h"

struct catnip_lua_resource_list* catnip_lua_keyboard_list = NULL;

void
catnip_lua_keyboard_list_init(lua_State* L)
{
  catnip_lua_keyboard_list = catnip_lua_resource_list_create(L);
}

void
catnip_lua_keyboard_list_populate(lua_State* L)
{
  struct catnip_keyboard* keyboard = NULL;
  wl_list_for_each(keyboard, &catnip_keyboards, link)
  {
    catnip_lua_keyboard_create(L, keyboard);
  }
}
