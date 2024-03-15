#include "lua_catnip.h"
#include "canvas/lua_canvas.h"
#include "config.h"
#include "cursor/lua_cursor.h"
#include "display.h"
#include "event_loop.h"
#include "keyboard/lua_keyboard_list.h"
#include "lua_event.h"
#include "lua_events.h"
#include "lua_resource.h"
#include "lua_resource_list.h"
#include "output/lua_output_list.h"
#include "utils/lua.h"
#include "utils/string.h"
#include "window/lua_window_list.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

static struct wl_event_source* lua_catnip_quit_event_source = NULL;
static struct wl_event_source* lua_catnip_reload_event_source = NULL;

static void
__lua_catnip_quit()
{
  lua_catnip_quit_event_source = NULL;
  catnip_display_run = false;
}

static int
lua_catnip_quit(lua_State* L)
{
  if (lua_catnip_quit_event_source != NULL) {
    return 0;
  }

  lua_catnip_quit_event_source =
    wl_event_loop_add_idle(catnip_event_loop, __lua_catnip_quit, NULL);

  lua_catnip_events_publish(L, lua_catnip_subscriptions, "quit", 0);

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
    lua_log_warning(L, "attempted to reload during startup, ignoring...");
    return 0;
  }

  lua_catnip_reload_event_source =
    wl_event_loop_add_idle(catnip_event_loop, __lua_catnip_reload, NULL);

  lua_catnip_events_publish(L, lua_catnip_subscriptions, "reload", 0);

  return 0;
}

static int
lua_catnip_subscribe(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_pushvalue(L, 2); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, lua_catnip_subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
lua_catnip_unsubscribe(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_pushvalue(L, 2); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, lua_catnip_subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
lua_catnip_publish(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    event,
    lua_gettop(L) - 1
  );

  return 0;
}

static const struct luaL_Reg lua_catnip_lib[] = {
  {"quit", lua_catnip_quit},
  {"reload", lua_catnip_reload},
  {"subscribe", lua_catnip_subscribe},
  {"unsubscribe", lua_catnip_unsubscribe},
  {"publish", lua_catnip_publish},
  {"canvas", lua_catnip_canvas},
  {NULL, NULL}
};

void
lua_catnip_init(lua_State* L)
{
  lua_getglobal(L, "package");

  lua_getfield(L, -1, "path");

  char* lua_path = strfmt(
    "%s/?.lua;%s/?/init.lua;%s",
    CATNIP_INSTALL_DIR,
    CATNIP_INSTALL_DIR,
    lua_popstring(L)
  );

  lua_pushstring(L, lua_path);
  lua_setfield(L, -2, "path");
  free(lua_path);

  lua_getfield(L, -1, "loaded");

  luaL_newlib(L, lua_catnip_lib);

  lua_catnip_event_init(L);
  lua_catnip_events_init(L);

  lua_catnip_resource_init(L);
  lua_catnip_resource_list_init(L);

  lua_catnip_keyboard_list_init(L); // must init after resource / events
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_keyboard_list->ref);
  lua_setfield(L, -2, "keyboards");

  lua_catnip_window_list_init(L); // must init after resource / events
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_window_list->ref);
  lua_setfield(L, -2, "windows");

  lua_catnip_output_list_init(L); // must init after resource / events
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_output_list->ref);
  lua_setfield(L, -2, "outputs");

  lua_catnip_cursor_init(L); // must init after resource
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_cursor->ref);
  lua_setfield(L, -2, "cursor");

  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);
}
