#include "lua_catnip.h"
#include "api/canvas.h"
#include "api/events.h"
#include "api/keybindings.h"
#include "api/png.h"
#include "api/svg.h"
#include "config/config.h"
#include "config/events.h"
#include "cursor/lua_cursor.h"
#include "display.h"
#include "events/event_loop.h"
#include "output/lua_output.h"
#include "utils/log.h"
#include "window/lua_window.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

lua_Ref lua_catnip = LUA_NOREF;

static struct wl_event_source* quit_event_source = NULL;
static struct wl_event_source* reload_event_source = NULL;

static void
__lua_catnip_quit()
{
  quit_event_source = NULL;
  config_events_publish("quit");
  wl_display_terminate(catnip_display);
}

static int
lua_catnip_quit(lua_State* L)
{
  if (quit_event_source == NULL) {
    quit_event_source = catnip_event_loop_once(__lua_catnip_quit, NULL);
  }

  return 0;
}

static void
__lua_catnip_reload()
{
  reload_event_source = NULL;
  config_events_publish("reload");
  catnip_config_reload();
}

static int
lua_catnip_reload(lua_State* L)
{
  if (catnip_config_loading) {
    log_warning("attempted to reload during startup, ignoring...");
    return 0;
  }

  if (reload_event_source == NULL) {
    reload_event_source = catnip_event_loop_once(__lua_catnip_reload, NULL);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_lib[] =
  {{"quit", lua_catnip_quit}, {"reload", lua_catnip_reload}, {NULL, NULL}};

void
lua_catnip_init(lua_State* L)
{
  luaL_newlib(L, lua_catnip_lib);
  lua_catnip = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip);
  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);

  lua_catnip_window_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_windows);
  lua_setfield(L, -2, "windows");
  lua_pop(L, 1);

  lua_catnip_output_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_outputs);
  lua_setfield(L, -2, "outputs");
  lua_pop(L, 1);

  lua_catnip_cursor_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_cursor);
  lua_setfield(L, -2, "cursor");
  lua_pop(L, 1);

  api_keybindings_init(L);
  api_events_init(L);
  api_canvas_init(L);
  api_svg_init(L);
  api_png_init(L);
}
