#ifndef CATNIP_LUA_CANVAS_METHODS_H
#define CATNIP_LUA_CANVAS_METHODS_H

#include <lua.h>

int
lua_catnip_canvas_method_clear(lua_State* L);

int
lua_catnip_canvas_method_png(lua_State* L);

int
lua_catnip_canvas_method_svg(lua_State* L);

int
lua_catnip_canvas_method_destroy(lua_State* L);

#endif
