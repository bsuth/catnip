#include "lua_catnip.h"
#include "canvas/lua_canvas.h"
#include "canvas/lua_png.h"
#include "canvas/lua_svg.h"
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

static struct wl_event_source* lua_catnip_quit_event_source = NULL;
static struct wl_event_source* lua_catnip_reload_event_source = NULL;

static void
__lua_catnip_quit()
{
  lua_catnip_quit_event_source = NULL;
  wl_display_terminate(catnip_display);
}

static int
lua_catnip_quit(lua_State* L)
{
  if (lua_catnip_quit_event_source != NULL) {
    return 0;
  }

  lua_catnip_quit_event_source =
    catnip_event_loop_once(__lua_catnip_quit, NULL);

  lua_catnip_events_call_publish(L, "quit", 0);

  return 0;
}

static void
__lua_catnip_reload()
{
  lua_catnip_reload_event_source = NULL;
  catnip_config_reload();
}

static int
lua_catnip_reload(lua_State* L)
{
  if (lua_catnip_reload_event_source != NULL) {
    return 0;
  }

  if (catnip_config_loading) {
    log_warning("attempted to reload during startup, ignoring...");
    return 0;
  }

  lua_catnip_reload_event_source =
    catnip_event_loop_once(__lua_catnip_reload, NULL);

  lua_catnip_events_call_publish(L, "reload", 0);

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

  lua_catnip_events_init(L);
  lua_pushcfunction(L, lua_catnip_events_global_subscribe);
  lua_setfield(L, -2, "subscribe");
  lua_pushcfunction(L, lua_catnip_events_global_unsubscribe);
  lua_setfield(L, -2, "unsubscribe");
  lua_pushcfunction(L, lua_catnip_events_global_publish);
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

  lua_catnip_canvas_init(L);
  lua_pushcfunction(L, lua_catnip_canvas_create);
  lua_setfield(L, -2, "canvas");

  lua_catnip_png_init(L);
  lua_pushcfunction(L, lua_catnip_png_create);
  lua_setfield(L, -2, "png");

  lua_catnip_svg_init(L);
  lua_pushcfunction(L, lua_catnip_svg_create);
  lua_setfield(L, -2, "svg");

  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);
}
