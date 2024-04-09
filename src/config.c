#include "config.h"
#include "default_config.h"
#include "lua_catnip.h"
#include "utils/log.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <libgen.h>
#include <linux/limits.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

lua_State* catnip_L = NULL;
char* catnip_config_user_path = NULL;
bool catnip_config_loading = false;

static char* catnip_config_xdg_path = NULL;
static char* catnip_config_default_path = NULL;

static bool
catnip_config_load(
  int (*load)(lua_State* L, const char* data),
  const char* data
)
{
  lua_State* new_catnip_L = luaL_newstate();
  luaL_openlibs(new_catnip_L);
  lua_catnip_init(new_catnip_L);

  catnip_config_loading = true;
  bool loaded = !load(new_catnip_L, data) && !lua_pcall(new_catnip_L, 0, 0, 0);
  catnip_config_loading = false;

  if (!loaded) {
    log_error("%s", lua_tostring(new_catnip_L, -1));
    lua_close(new_catnip_L);
    return false;
  }

  lua_catnip_populate(new_catnip_L);

  if (catnip_L != NULL) {
    lua_close(catnip_L);
  }

  catnip_L = new_catnip_L;

  return true;
}

static bool
catnip_config_load_path(const char* path)
{
  if (access(path, R_OK) == -1) {
    return false;
  }

  char* cwd = getcwd(NULL, 0);
  char* parent = strdup(path);

  chdir(dirname(parent));

  bool loaded = catnip_config_load(luaL_loadfile, path);

  if (!loaded) {
    chdir(cwd);
  }

  free(cwd);
  free(parent);

  return loaded;
}

bool
catnip_config_reload()
{
  if (catnip_config_user_path != NULL) {
    return catnip_config_load_path(catnip_config_user_path);
  } else if (catnip_config_xdg_path != NULL) {
    return catnip_config_load_path(catnip_config_xdg_path);
  } else {
    return catnip_config_load_path(catnip_config_default_path);
  }
}

void
catnip_config_init()
{
  const char* xdg_config_home = getenv("XDG_CONFIG_HOME");

  if (xdg_config_home != NULL && xdg_config_home[0] != '\0') {
    catnip_config_xdg_path = strfmt("%s/catnip/init.lua", xdg_config_home);
  }

  const char* home = getenv("HOME");

  if (home != NULL && home[0] != '\0') {
    catnip_config_default_path = strfmt("%s/.config/catnip/init.lua", home);
  }

  if (!catnip_config_reload()) {
    catnip_config_load(luaL_loadstring, catnip_default_config);
  }
}
