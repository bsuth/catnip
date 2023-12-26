#include "config.h"
#include "lua_catnip.h"
#include "meta.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
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

  char* current_dir = g_get_current_dir();

  char* config_dir = g_path_get_dirname(path);
  chdir(config_dir);
  free(config_dir);

  lua_State* new_catnip_L = lua_open();
  luaL_openlibs(new_catnip_L);
  lua_catnip_init(new_catnip_L);

  char* config_basename = g_path_get_basename(path);
  catnip_config_loading = true;

  bool loaded = !luaL_loadfile(new_catnip_L, config_basename)
                && !lua_pcall(new_catnip_L, 0, 0, 0);

  catnip_config_loading = false;
  free(config_basename);

  chdir(current_dir);
  free(current_dir);

  if (!loaded) {
    log_error("%s: %s", path, lua_tostring(new_catnip_L, -1));
    lua_close(new_catnip_L);
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
    gchar* path = g_strconcat(xdg_config_home, "/catnip/init.lua", NULL);
    bool loaded = catnip_config_try(path);
    g_free(path);

    if (loaded) {
      return true;
    }
  }

  const char* home = getenv("HOME");

  if (home != NULL && home[0] != '\0') {
    gchar* path = g_strconcat(home, "/.config/catnip/init.lua", NULL);
    bool loaded = catnip_config_try(path);
    g_free(path);

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
    catnip_config_try(CATNIP_INSTALL_DIR "/lua/fallback.lua");
  }
}

void
catnip_config_reload()
{
  catnip_config_load();
}
