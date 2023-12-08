#ifndef CATNIP_LUA_OUTPUT_EVENTS_H
#define CATNIP_LUA_OUTPUT_EVENTS_H

#include "output/output.h"
#include <lua.h>

int
lua_catnip_output_subscribe(lua_State* L);

int
lua_catnip_output_unsubscribe(lua_State* L);

int
lua_catnip_output_publish(lua_State* L);

void
lua_catnip_output_call_publish(
  lua_State* L,
  struct catnip_output* output,
  const char* event,
  int nargs
);

#endif
