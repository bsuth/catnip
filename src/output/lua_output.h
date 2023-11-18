#ifndef CATNIP_LUA_OUTPUT_H
#define CATNIP_LUA_OUTPUT_H

#include "output.h"
#include "utils/lua.h"

extern lua_Ref lua_catnip_outputs;

void
lua_catnip_output_destroy(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_create(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_init(lua_State* L);

#endif
