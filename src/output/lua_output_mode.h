#ifndef CATNIP_LUA_OUTPUT_MODE_H
#define CATNIP_LUA_OUTPUT_MODE_H

#include "output/output.h"
#include <lua.h>

void
lua_catnip_output_modes_destroy(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_modes_create(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_mode_init(lua_State* L);

#endif
