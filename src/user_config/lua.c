#include "lua.h"
#include "../config.h"
#include "../utils/string.h"
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wlr/util/log.h>

lua_State* L;

// TODO: Allow the user to manipulate this. This should be settable both via the
// CLI and via the Lua API. In the latter case, this will be especially useful
// for dynamically switching the theme.
//
// 1. Bash: `bwc -c my_custom_config.lua`
// 2. Lua: `require('bwc').restart("my_custom_config.lua")`
// 3. Lua: `require('bwc').restart("")` -- let bwc auto detect
char* user_config_path;

static void
try_config_path(const char* path)
{
  if (path == NULL || path[0] == '\0' || access(path, R_OK) == -1) {
    return;
  }

  L = lua_open();
  luaL_openlibs(L);

  if (luaL_loadfile(L, path) == 0) {
    lua_call(L, 0, 0);
  } else {
    wlr_log(WLR_ERROR, "%s", lua_tostring(L, -1));
    lua_close(L);
    L = NULL;
  }
}

void
restart_user_config()
{
  try_config_path(user_config_path);

  if (L != NULL) {
    return;
  }

  char* default_config_path = getenv("XDG_CONFIG_HOME");
  const char* env_xdg_config_home = getenv("XDG_CONFIG_HOME");
  if (env_xdg_config_home != NULL && env_xdg_config_home[0] != '\0') {
    default_config_path = string_concat(env_xdg_config_home, "/bwc/init.lua");
  } else {
    const char* env_home = getenv("HOME");
    if (env_home != NULL && env_home[0] != '\0') {
      default_config_path = string_concat(env_home, "/.config/bwc/init.lua");
    }
  }

  try_config_path(default_config_path);
  free(default_config_path);

  if (L != NULL) {
    return;
  }

  try_config_path(ROOT_DIR "/fallback_config/init.lua");
}
