#ifndef CONFIG_LUA_H
#define CONFIG_LUA_H

#include <lua.h>

extern lua_State* L;

void
restart_user_config();

#endif
