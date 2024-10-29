#include "lua_output.h"
#include "desktop/cursor.h"
#include "desktop/lua_output_mode.h"
#include "desktop/lua_output_modes.h"
#include "desktop/lua_outputs.h"
#include "desktop/outputs.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Lua Methods
// -----------------------------------------------------------------------------

static int
catnip_lua_output_lua_on(lua_State* L)
{
  struct catnip_lua_output* lua_output = luaL_checkudata(L, 1, "catnip.output");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, lua_output->subscriptions, event);

  return 1;
}

static int
catnip_lua_output_lua_emit(lua_State* L)
{
  struct catnip_lua_output* lua_output = luaL_checkudata(L, 1, "catnip.output");
  const char* event = luaL_checkstring(L, 2);

  catnip_lua_events_publish(
    L,
    lua_output->subscriptions,
    event,
    lua_gettop(L) - 2
  );

  return 0;
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static void
catnip_lua_output_push_mode(
  lua_State* L,
  struct catnip_lua_output* lua_output,
  struct wlr_output_mode* wlr_output_mode
)
{
  struct catnip_lua_output_mode* lua_output_mode = NULL;
  wl_list_for_each(lua_output_mode, &lua_output->lua_output_modes->modes, link)
  {
    if (lua_output_mode->wlr_output_mode == wlr_output_mode) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, lua_output_mode->ref);
      return;
    }
  }

  lua_pushnil(L);
}

static int
catnip_lua_output__index(lua_State* L)
{
  struct catnip_lua_output* lua_output = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  struct catnip_output* output = lua_output->output;

  if (output == NULL) {
    lua_log_error(L, "attempt to index outdated output");
    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "id")) {
    lua_pushnumber(L, output->id);
  } else if (streq(key, "on")) {
    lua_pushcfunction(L, catnip_lua_output_lua_on);
  } else if (streq(key, "emit")) {
    lua_pushcfunction(L, catnip_lua_output_lua_emit);
  } else if (streq(key, "x")) {
    lua_pushnumber(
      L,
      wlr_output_layout_get(catnip_output_layout, output->wlr.output)->x
    );
  } else if (streq(key, "y")) {
    lua_pushnumber(
      L,
      wlr_output_layout_get(catnip_output_layout, output->wlr.output)->y
    );
  } else if (streq(key, "width")) {
    lua_pushnumber(L, output->wlr.output->width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, output->wlr.output->height);
  } else if (streq(key, "refresh")) {
    lua_pushnumber(L, output->wlr.output->refresh);
  } else if (streq(key, "mode")) {
    catnip_lua_output_push_mode(
      L,
      lua_output,
      output->wlr.output->current_mode
    );
  } else if (streq(key, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_output->lua_output_modes->ref);
  } else if (streq(key, "scale")) {
    lua_pushnumber(L, output->wlr.output->scale);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_output__newindex(lua_State* L)
{
  struct catnip_lua_output* lua_output = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  struct catnip_output* output = lua_output->output;

  if (key == NULL) {
    return 0;
  }

  if (output == NULL) {
    lua_log_error(L, "attempt to index outdated output");
  } else if (streq(key, "x")) {
    wlr_output_layout_add(
      catnip_output_layout,
      output->wlr.output,
      luaL_checknumber(L, 3),
      wlr_output_layout_get(catnip_output_layout, output->wlr.output)->y
    );
  } else if (streq(key, "y")) {
    wlr_output_layout_add(
      catnip_output_layout,
      output->wlr.output,
      wlr_output_layout_get(catnip_output_layout, output->wlr.output)->x,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "width")) {
    catnip_output_configure(output, luaL_checknumber(L, 3), -1, -1);
  } else if (streq(key, "height")) {
    catnip_output_configure(output, -1, luaL_checknumber(L, 3), -1);
  } else if (streq(key, "refresh")) {
    catnip_output_configure(output, -1, -1, luaL_checknumber(L, 3));
  } else if (streq(key, "mode")) {
    struct catnip_lua_output_mode* lua_output_mode =
      luaL_checkudata(L, 3, "catnip.output.mode");

    struct wlr_output_state state = {0};
    wlr_output_state_set_mode(&state, lua_output_mode->wlr_output_mode);

    wlr_output_commit_state(output->wlr.output, &state);
    wlr_output_state_finish(&state);
  } else if (streq(key, "scale")) {
    struct wlr_output_state state = {0};
    int new_scale = luaL_checknumber(L, 3);

    wlr_output_state_set_scale(&state, new_scale);
    wlr_output_commit_state(output->wlr.output, &state);
    wlr_output_state_finish(&state);

    // Ensure we have loaded a scaled cursor theme for the new output's scale
    wlr_xcursor_manager_load(catnip_xcursor_manager, new_scale);
  }

  return 0;
}

static const struct luaL_Reg catnip_lua_output_mt[] = {
  {"__index", catnip_lua_output__index},
  {"__newindex", catnip_lua_output__newindex},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_output_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.output");
  luaL_setfuncs(L, catnip_lua_output_mt, 0);
  lua_pop(L, 1);
}

void
catnip_lua_output_create(lua_State* L, struct catnip_output* output)
{
  struct catnip_lua_output* lua_output =
    lua_newuserdata(L, sizeof(struct catnip_lua_output));
  luaL_setmetatable(L, "catnip.output");

  lua_output->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_output->output = output;
  lua_output->lua_output_modes = catnip_lua_output_modes_create(L, output);

  lua_newtable(L);
  lua_output->subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  wl_list_insert(&catnip_lua_outputs->outputs, &lua_output->link);

  // Assign `output->lua_output` here, since the Lua output may be created much
  // later than the actual output (ex. on config reload).
  output->lua_output = lua_output;

  catnip_lua_output_publish(L, lua_output, "create", 0);
}

void
catnip_lua_output_destroy(lua_State* L, struct catnip_lua_output* lua_output)
{
  catnip_lua_output_publish(L, lua_output, "destroy", 0);

  // Explicitly set `NULL`, just in case the user is keeping a reference.
  lua_output->output = NULL;

  catnip_lua_output_modes_destroy(L, lua_output->lua_output_modes);
  luaL_unref(L, LUA_REGISTRYINDEX, lua_output->subscriptions);
  luaL_unref(L, LUA_REGISTRYINDEX, lua_output->ref);
  wl_list_remove(&lua_output->link);
}

void
catnip_lua_output_publish(
  lua_State* L,
  struct catnip_lua_output* lua_output,
  const char* event,
  int nargs
)
{
  catnip_lua_events_publish(L, lua_output->subscriptions, event, nargs);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_output->ref);
  lua_insert(L, -1 - nargs);

  catnip_lua_events_publish(
    L,
    catnip_lua_outputs->subscriptions,
    event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
}
