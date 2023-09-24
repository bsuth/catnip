#ifndef CATNIP_USER_CONFIG_H
#define CATNIP_USER_CONFIG_H

#include <lua.h>
#include <stdbool.h>

extern lua_State* L;

// Reloading the user config requires closing the current lua_State and opening
// a new one, something we cannot do while executing a Lua callback (without
// either throwing an error or segfaulting, both of which are undesirable).
// Thus, to allow users to reload the user config from within Lua, we simply set
// a flag, which is then checked after the callback has already returned.
extern bool user_config_request_reload;

void
init_user_config();

void
reload_user_config();

#endif