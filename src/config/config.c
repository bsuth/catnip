#include "config.h"
#include "api/api.h"
#include "config/events.h"
#include "config/keybindings.h"
#include "meta.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <lualib.h>
#include <unistd.h>

lua_State* L;

bool config_reload_requested = false;

// TODO: Allow the user to manipulate this. This should be settable both via the
// CLI and via the Lua API. In the latter case, this will be especially useful
// for dynamically switching the theme.
//
// 1. Bash: `catnip -c my_custom_config.lua`
// 2. Lua: `require('catnip').reload("my_custom_config.lua")`
// 3. Lua: `require('catnip').reload("")` -- let catnip auto detect
static char* config_path;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static bool
config_try_path(const char* path)
{
  if (path == NULL || path[0] == '\0' || access(path, R_OK) == -1) {
    return false;
  }

  L = lua_open();
  luaL_openlibs(L);
  api_init(L);

  if (luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0)) {
    log_error("%s", lua_tostring(L, -1));
    lua_close(L);
    L = NULL;
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// Config
// -----------------------------------------------------------------------------

void
config_load()
{
  if (config_try_path(config_path)) {
    return;
  }

  gchar* default_config_path;
  const char* env_xdg_config_home = getenv("XDG_CONFIG_HOME");
  if (env_xdg_config_home != NULL && env_xdg_config_home[0] != '\0') {
    default_config_path =
      g_strconcat(env_xdg_config_home, "/catnip/init.lua", NULL);
  } else {
    const char* env_home = getenv("HOME");
    if (env_home != NULL && env_home[0] != '\0') {
      default_config_path =
        g_strconcat(env_home, "/.config/catnip/init.lua", NULL);
    }
  }

  if (!config_try_path(default_config_path)) {
    config_try_path(ROOT_DIR "/lua/fallback.lua");
  }

  g_free(default_config_path);
}

void
config_reload()
{
  config_events_publish("reload");

  config_keybindings_clear();
  config_events_clear_subscriptions(NULL);

  lua_close(L);
  L = NULL;

  config_load();
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
config_init()
{
  config_keybindings_init();
  config_events_init();
}
