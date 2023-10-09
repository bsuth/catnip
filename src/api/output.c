#include "output.h"
#include "api/api.h"
#include "config/config.h"
#include "server/output.h"
#include "utils/log.h"
#include "utils/lua.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <string.h>
#include <wayland-util.h>

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

struct api_output {
  struct server_output* server_output;

  int index;
  lua_Ref modes;

  struct {
    struct wl_listener server_output_destroy;
  } listeners;
};

struct api_output_mode {
  struct wlr_output_mode* mode;
};

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

lua_Ref api_catnip_outputs = LUA_NOREF;

static struct {
  struct wl_listener new_server_output;
} listeners;

// Keep track of num_outputs manually to simplify managing Lua's catnip.outputs.
static int num_outputs = 0;

// -----------------------------------------------------------------------------
// Metatable: catnip.output.mode
// -----------------------------------------------------------------------------

static int
api_output_mode__index(lua_State* L)
{
  struct api_output_mode* api_output_mode = lua_touserdata(L, 1);
  struct wlr_output_mode* mode = api_output_mode->mode;

  if (mode == NULL) {
    log_error("cannot get field of expired mode");
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "width")) {
    lua_pushnumber(L, mode->width);
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, mode->height);
  } else if (g_str_equal(key, "refresh")) {
    lua_pushnumber(L, mode->refresh);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static const struct luaL_Reg api_output_mode_metatable[] = {
  {"__index", api_output_mode__index},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// Metatable: catnip.output
// -----------------------------------------------------------------------------

static void
api_output_push_current_mode(lua_State* L, struct api_output* api_output)
{
  struct wlr_output_mode* current_mode =
    server_output_get_mode(api_output->server_output);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_output->modes);
  lua_pushnil(L);

  while (lua_next(L, -2) != 0) {
    struct api_output_mode* api_output_mode =
      luaL_checkudata(L, -2, "catnip.output.mode");

    if (api_output_mode->mode == current_mode) {
      lua_pop(L, 1);
      lua_remove(L, -2);
      return;
    }
  }

  lua_pop(L, 1);
  lua_pushnil(L);
}

static int
api_output__index(lua_State* L)
{
  struct api_output* api_output = lua_touserdata(L, 1);
  struct server_output* server_output = api_output->server_output;

  if (server_output == NULL) {
    log_warning("cannot get field of expired output");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, server_output_get_x(server_output));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, server_output_get_y(server_output));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, server_output_get_width(server_output));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, server_output_get_height(server_output));
  } else if (g_str_equal(key, "refresh")) {
    lua_pushnumber(L, server_output_get_refresh(server_output));
  } else if (g_str_equal(key, "mode")) {
    api_output_push_current_mode(L, api_output);
  } else if (g_str_equal(key, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, api_output->modes);
  } else if (g_str_equal(key, "scale")) {
    lua_pushnumber(L, server_output_get_scale(server_output));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_output__newindex(lua_State* L)
{
  struct api_output* api_output = lua_touserdata(L, 1);
  struct server_output* server_output = api_output->server_output;

  if (server_output == NULL) {
    log_warning("cannot set field of expired output");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    log_warning("invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    server_output_set_x(server_output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    server_output_set_y(server_output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    server_output_set_width(server_output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    server_output_set_height(server_output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "refresh")) {
    server_output_set_refresh(server_output, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "mode")) {
    struct api_output_mode* api_output_mode =
      luaL_checkudata(L, 3, "catnip.output.mode");
    server_output_set_mode(server_output, api_output_mode->mode);
  } else if (g_str_equal(key, "scale")) {
    server_output_set_scale(server_output, luaL_checknumber(L, 3));
  } else {
    log_warning("invalid key: %s", key);
  }

  return 0;
}

static int
api_output__gc(lua_State* L)
{
  struct api_output* api_output = lua_touserdata(L, 1);

  wl_list_remove(&api_output->listeners.server_output_destroy.link);

  return 0;
}

static const struct luaL_Reg api_output_metatable[] = {
  {"__gc", api_output__gc},
  {"__index", api_output__index},
  {"__newindex", api_output__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_server_output_destroy(struct wl_listener* listener, void* data)
{
  struct api_output* api_output =
    wl_container_of(listener, api_output, listeners.server_output_destroy);

  api_output->server_output = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, api_output->modes);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_outputs);

  if (num_outputs == 1) {
    lua_pushnil(L);
    lua_rawseti(L, -2, 1);
  } else {
    // Quick delete by moving last element
    lua_rawgeti(L, -1, num_outputs);

    struct api_output* last_api_output = lua_touserdata(L, -1);
    last_api_output->index = api_output->index;

    lua_rawseti(L, -2, api_output->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, num_outputs);
  }

  --num_outputs;

  lua_pop(L, 1);
}

static void
on_new_server_output(struct wl_listener* listener, void* data)
{
  struct server_output* server_output = data;

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_outputs);

  struct api_output* api_output = lua_newuserdata(L, sizeof(struct api_output));
  luaL_setmetatable(L, "catnip.output");

  ++num_outputs;
  api_output->server_output = server_output;
  api_output->index = num_outputs;

  lua_newtable(L);
  struct wlr_output_mode* mode;
  struct wl_list* modes = server_output_get_modes(server_output);
  int mode_counter = 0;
  wl_list_for_each(mode, modes, link)
  {
    struct api_output_mode* api_output_mode =
      lua_newuserdata(L, sizeof(struct api_output_mode));
    luaL_setmetatable(L, "catnip.output.mode");
    api_output_mode->mode = mode;
    lua_rawseti(L, -2, ++mode_counter);
  }
  api_output->modes = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_setup_listener(
    &api_output->listeners.server_output_destroy,
    &server_output->events.destroy,
    on_server_output_destroy
  );

  lua_rawseti(L, -2, api_output->index);

  lua_pop(L, 1);
}

void
init_api_outputs(lua_State* L)
{
  lua_newtable(L);
  api_catnip_outputs = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_outputs);
  lua_setfield(L, -2, "outputs");
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.output");
  luaL_setfuncs(L, api_output_metatable, 0);
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.output.mode");
  luaL_setfuncs(L, api_output_mode_metatable, 0);
  lua_pop(L, 1);

  wl_setup_listener(
    &listeners.new_server_output,
    &server_output_events.new_server_output,
    on_new_server_output
  );
}
