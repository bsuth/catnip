#include "config.h"
#include "default_config.h"
#include "extensions/string.h"
#include "log.h"
#include "lua_events.h"
#include "lua_state.h"
#include <lauxlib.h>
#include <libgen.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

lua_State* catnip_L = NULL;
char* catnip_config_user_path = NULL;
bool catnip_config_loading = false;
bool catnip_config_request_reload = false;

static char* catnip_config_xdg_path = NULL;
static char* catnip_config_default_path = NULL;

static void
catnip_config_load(
  int (*load)(lua_State* L, const char* data),
  const char* data
)
{
  if (catnip_L != NULL) {
    lua_close(catnip_L);
  }

  catnip_L = luaL_newstate();
  luaL_openlibs(catnip_L);
  catnip_lua_state_init(catnip_L);

  catnip_config_loading = true;
  bool loaded = !load(catnip_L, data) && !lua_pcall(catnip_L, 0, 0, 0);
  catnip_config_loading = false;

  if (!loaded) {
    catnip_log_error("%s", lua_tostring(catnip_L, -1));
    lua_close(catnip_L);
    catnip_L = NULL;
  }

  catnip_lua_state_populate(catnip_L);
}

static void
catnip_config_load_path(const char* path)
{
  if (access(path, R_OK) == -1) {
    return;
  }

  char* cwd = getcwd(NULL, 0);
  char* parent = strdup(path);

  chdir(dirname(parent));
  catnip_config_load(luaL_loadfile, path);

  if (catnip_L == NULL) {
    chdir(cwd);
  }

  free(cwd);
  free(parent);
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

  catnip_config_reload();
}

void
catnip_config_reload()
{
  if (catnip_L != NULL) {
    catnip_lua_events_publish(catnip_L, catnip_lua_subscriptions, "reload", 0);

    // Set this _after_ publishing the event, just in case somebody calls
    // `catnip.reload()` during the `reload` event...
    catnip_config_request_reload = false;

    lua_close(catnip_L);
    catnip_L = NULL;
  }

  if (catnip_config_user_path != NULL) {
    catnip_config_load_path(catnip_config_user_path);
  }

  if (catnip_L == NULL && catnip_config_xdg_path != NULL) {
    catnip_config_load_path(catnip_config_xdg_path);
  }

  if (catnip_L == NULL && catnip_config_default_path != NULL) {
    catnip_config_load_path(catnip_config_default_path);
  }

  if (catnip_L == NULL) {
    catnip_config_load(luaL_loadstring, catnip_default_config);
  }
}
