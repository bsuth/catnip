#ifndef CATNIP_CONFIG_H
#define CATNIP_CONFIG_H

#include <lua.h>
#include <stdbool.h>

extern lua_State* catnip_L;
extern char* catnip_config_user_path;
extern bool catnip_config_loading;
extern bool catnip_config_request_reload;

void
catnip_config_init();

bool
catnip_config_reload();

#endif
