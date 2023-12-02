#include "lua_catnip.h"
#include "api/canvas.h"
#include "api/png.h"
#include "api/svg.h"
#include "config/config.h"
#include "cursor/lua_cursor.h"
#include "display.h"
#include "events/event_loop.h"
#include "events/lua_events.h"
#include "input/lua_key_event.h"
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
  wl_display_terminate(catnip_display);
}

static int
lua_catnip_quit(lua_State* L)
{
  if (quit_event_source != NULL) {
    return 0;
  }

  quit_event_source = catnip_event_loop_once(__lua_catnip_quit, NULL);

  lua_pushcfunction(catnip_L, catnip_lua_events_global_publish);
  lua_pushstring(catnip_L, "quit");
  lua_call(catnip_L, 1, 0);

  return 0;
}

static void
__lua_catnip_reload()
{
  reload_event_source = NULL;
  catnip_config_reload();
}

static int
lua_catnip_reload(lua_State* L)
{
  if (reload_event_source != NULL) {
    return 0;
  }

  if (catnip_config_loading) {
    log_warning("attempted to reload during startup, ignoring...");
    return 0;
  }

  lua_pushcfunction(catnip_L, catnip_lua_events_global_publish);
  lua_pushstring(catnip_L, "reload");
  lua_call(catnip_L, 1, 0);

  return 0;
}

static const struct luaL_Reg lua_catnip_lib[] =
  {{"quit", lua_catnip_quit}, {"reload", lua_catnip_reload}, {NULL, NULL}};

void
lua_catnip_init(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");

  luaL_newlib(L, lua_catnip_lib);

  // TODO: remove this? dont actually need to store reference to lua_catnip
  lua_pushvalue(L, -1);
  lua_catnip = luaL_ref(L, LUA_REGISTRYINDEX);

  catnip_lua_events_init(L);
  lua_pushcfunction(L, catnip_lua_events_global_subscribe);
  lua_setfield(L, -2, "subscribe");
  lua_pushcfunction(L, catnip_lua_events_global_unsubscribe);
  lua_setfield(L, -2, "unsubscribe");
  lua_pushcfunction(L, catnip_lua_events_global_publish);
  lua_setfield(L, -2, "publish");

  lua_catnip_key_event_init(L);

  lua_catnip_window_init(L); // must init after events
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_windows);
  lua_setfield(L, -2, "windows");

  lua_catnip_output_init(L); // must init after events
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_outputs);
  lua_setfield(L, -2, "outputs");

  lua_catnip_cursor_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_cursor);
  lua_setfield(L, -2, "cursor");

  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);

  api_canvas_init(L);
  api_svg_init(L);
  api_png_init(L);
}
