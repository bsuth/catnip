#ifndef CATNIP_CONFIG_H
#define CATNIP_CONFIG_H

#include <lua.h>
#include <stdbool.h>

extern lua_State* catnip_L;
extern char* catnip_config_path;
extern bool catnip_config_loading;

void
catnip_config_init();

void
catnip_config_reload();

#endif
