#include "config.h"
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
char* catnip_config_path = NULL;
bool catnip_config_loading = false;

static bool
catnip_config_try(const char* path)
{
  if (access(path, R_OK) == -1) {
    return false;
  }

  char* path_copy;

  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  path_copy = strdup(path);
  chdir(dirname(path_copy));
  free(path_copy);

  lua_State* new_catnip_L = luaL_newstate();
  luaL_openlibs(new_catnip_L);
  lua_catnip_init(new_catnip_L);

  path_copy = strdup(path);
  catnip_config_loading = true;
  bool loaded = !luaL_loadfile(new_catnip_L, basename(path_copy))
    && !lua_pcall(new_catnip_L, 0, 0, 0);
  catnip_config_loading = false;
  free(path_copy);

  if (!loaded) {
    log_error("%s", lua_tostring(new_catnip_L, -1));
    lua_close(new_catnip_L);
    chdir(cwd);
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
catnip_config_load()
{
  if (catnip_config_path != NULL && catnip_config_try(catnip_config_path)) {
    return true;
  }

  const char* xdg_config_home = getenv("XDG_CONFIG_HOME");
  char* default_config_path_1 = NULL;

  if (xdg_config_home != NULL && xdg_config_home[0] != '\0') {
    default_config_path_1 = strfmt("%s/catnip/init.lua", xdg_config_home);

    if (catnip_config_try(default_config_path_1)) {
      free(default_config_path_1);
      return true;
    }
  }

  const char* home = getenv("HOME");
  char* default_config_path_2 = NULL;

  if (home != NULL && home[0] != '\0') {
    default_config_path_2 = strfmt("%s/.config/catnip/init.lua", home);

    if (default_config_path_1 == NULL) {
      if (catnip_config_try(default_config_path_2)) {
        free(default_config_path_2);
        return true;
      }
    } else if (!streq(default_config_path_1, default_config_path_2)) {
      if (catnip_config_try(default_config_path_2)) {
        free(default_config_path_1);
        free(default_config_path_2);
        return true;
      }
    }
  }

  if (default_config_path_1 != NULL) {
    free(default_config_path_1);
  }

  if (default_config_path_2 != NULL) {
    free(default_config_path_2);
  }

  return false;
}

void
catnip_config_init()
{
  if (!catnip_config_load()) {
    catnip_config_try(CATNIP_INSTALL_DIR "/catmint/default_config.lua");
  }
}

void
catnip_config_reload()
{
  catnip_config_load();
}
