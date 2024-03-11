#ifndef CATNIP_LUA_OUTPUT_H
#define CATNIP_LUA_OUTPUT_H

#include "output/output.h"

void
lua_catnip_output_destroy(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_create(lua_State* L, struct catnip_output* output);

void
lua_catnip_output_publish(
  lua_State* L,
  struct catnip_output* output,
  const char* event,
  int nargs
);

void
lua_catnip_output_init(lua_State* L);

#endif
