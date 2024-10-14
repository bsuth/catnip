#include "lua_widget.h"
#include "extensions/string.h"
#include "widget/lua_widget_block.h"
#include "widget/lua_widget_png.h"
#include "widget/lua_widget_root.h"
#include "widget/lua_widget_svg.h"
#include "widget/lua_widget_text.h"
#include <lauxlib.h>

lua_Ref catnip_lua_widget;

static int
catnip_lua_widget__index(lua_State* L)
{
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "root")) {
    lua_pushcfunction(L, catnip_lua_widget_lua_root);
  } else if (streq(key, "block")) {
    lua_pushcfunction(L, catnip_lua_widget_lua_block);
  } else if (streq(key, "png")) {
    lua_pushcfunction(L, catnip_lua_widget_lua_png);
  } else if (streq(key, "svg")) {
    lua_pushcfunction(L, catnip_lua_widget_lua_svg);
  } else if (streq(key, "text")) {
    lua_pushcfunction(L, catnip_lua_widget_lua_text);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static const struct luaL_Reg catnip_lua_widget_mt[] = {
  {"__index", catnip_lua_widget__index},
  {NULL, NULL}
};

void
catnip_lua_widget_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget");
  luaL_setfuncs(L, catnip_lua_widget_mt, 0);
  lua_pop(L, 1);

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.widget");
  catnip_lua_widget = luaL_ref(L, LUA_REGISTRYINDEX);
}
