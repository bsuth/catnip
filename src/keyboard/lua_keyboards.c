#include "lua_keyboards.h"
#include "keyboard/lua_keyboard.h"
#include <lauxlib.h>

lua_Ref lua_catnip_keyboards = LUA_NOREF;

static int
lua_catnip_keyboards__index(lua_State* L)
{
  int id = luaL_checknumber(L, 2);

  struct catnip_keyboard* keyboard = NULL;
  wl_list_for_each(keyboard, &catnip_keyboards, link)
  {
    if (keyboard->id == id) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard->lua.ref);
      return 1;
    }
  }

  lua_pushnil(L);
  return 1;
}

static int
lua_catnip_keyboards__call(lua_State* L)
{
  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? catnip_keyboards.next
    : (*(struct catnip_keyboard**) luaL_checkudata(L, 3, "catnip.keyboard"))
        ->link.next;

  if (link == &catnip_keyboards) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_keyboard* keyboard = wl_container_of(link, keyboard, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard->lua.ref);
  return 1;
}

static const struct luaL_Reg lua_catnip_keyboards_mt[] = {
  {"__index", lua_catnip_keyboards__index},
  {"__call", lua_catnip_keyboards__call},
  {NULL, NULL}
};

void
lua_catnip_keyboards_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.keyboards");
  luaL_setfuncs(L, lua_catnip_keyboards_mt, 0);
  lua_pop(L, 1);

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.keyboards");
  lua_catnip_keyboards = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_catnip_keyboard_init(L);

  struct catnip_keyboard* keyboard = NULL;
  wl_list_for_each(keyboard, &catnip_keyboards, link)
  {
    lua_catnip_keyboard_create(L, keyboard);
  }
}
