#include "user_config.h"
#include "../config.h"
#include "../lua_api/lua_api.h"
#include <glib.h>
#include <lauxlib.h>
#include <lualib.h>
#include <unistd.h>

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
try_user_config_path(const char* path)
{
  if (path == NULL || path[0] == '\0' || access(path, R_OK) == -1) {
    return;
  }

  L = lua_open();
  luaL_openlibs(L);
  init_lua_api(L);

  if (luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0)) {
    g_error("%s", lua_tostring(L, -1));
    lua_close(L);
    L = NULL;
  }
}

void
reload_user_config()
{
  try_user_config_path(user_config_path);

  if (L != NULL) {
    return;
  }

  gchar* default_config_path;
  const char* env_xdg_config_home = getenv("XDG_CONFIG_HOME");
  if (env_xdg_config_home != NULL && env_xdg_config_home[0] != '\0') {
    default_config_path =
      g_strconcat(env_xdg_config_home, "/bwc/init.lua", NULL);
  } else {
    const char* env_home = getenv("HOME");
    if (env_home != NULL && env_home[0] != '\0') {
      default_config_path =
        g_strconcat(env_home, "/.config/bwc/init.lua", NULL);
    }
  }

  try_user_config_path(default_config_path);
  g_free(default_config_path);

  if (L != NULL) {
    return;
  }

  try_user_config_path(ROOT_DIR "/fallback_config/init.lua");
}
