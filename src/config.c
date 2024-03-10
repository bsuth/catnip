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

  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  char* path_dirname_copy = strdup(path);
  chdir(dirname(path_dirname_copy));
  free(path_dirname_copy);

  lua_State* new_catnip_L = lua_open();
  luaL_openlibs(new_catnip_L);
  lua_catnip_init(new_catnip_L);

  catnip_config_loading = true;
  char* path_basename_copy = strdup(path);
  const char* path_basename = basename(path_basename_copy);
  bool loaded = !luaL_loadfile(new_catnip_L, path_basename)
    && !lua_pcall(new_catnip_L, 0, 0, 0);
  free(path_basename_copy);
  catnip_config_loading = false;

  if (!loaded) {
    log_error("%s: %s", path, lua_tostring(new_catnip_L, -1));
    lua_close(new_catnip_L);
    chdir(cwd);
    return false;
  }

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

  if (xdg_config_home != NULL && xdg_config_home[0] != '\0') {
    char* path = strfmt("%s/catnip/init.lua", xdg_config_home);
    bool loaded = catnip_config_try(path);
    free(path);

    if (loaded) {
      return true;
    }
  }

  const char* home = getenv("HOME");

  if (home != NULL && home[0] != '\0') {
    char* path = strfmt("%s/.config/catnip/init.lua", home);
    bool loaded = catnip_config_try(path);
    free(path);

    if (loaded) {
      return true;
    }
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
