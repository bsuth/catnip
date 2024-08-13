#include "lua_output.h"
#include "desktop/cursor.h"
#include "desktop/lua_output_mode.h"
#include "desktop/lua_outputs.h"
#include "desktop/outputs.h"
#include "extensions/string.h"
#include "lua_events.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static void
catnip_lua_output_push_mode(
  lua_State* L,
  struct catnip_output* output,
  struct wlr_output_mode* mode
)
{
  struct catnip_lua_resource* lua_resource = NULL;
  wl_list_for_each(lua_resource, &output->lua_mode_list->head, link)
  {
    if (lua_resource->data == mode) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
      return;
    }
  }

  lua_pushnil(L);
}

static bool
catnip_lua_output__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_output* output = lua_resource->data;

  if (streq(key, "id")) {
    lua_pushnumber(L, output->id);
  } else if (streq(key, "x")) {
    int x = wlr_output_layout_get(catnip_output_layout, output->wlr.output)->x;
    lua_pushnumber(L, x);
  } else if (streq(key, "y")) {
    int y = wlr_output_layout_get(catnip_output_layout, output->wlr.output)->y;
    lua_pushnumber(L, y);
  } else if (streq(key, "width")) {
    lua_pushnumber(L, output->wlr.output->width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, output->wlr.output->height);
  } else if (streq(key, "refresh")) {
    lua_pushnumber(L, output->wlr.output->refresh);
  } else if (streq(key, "mode")) {
    output->wlr.output->current_mode != NULL
      ? catnip_lua_output_push_mode(L, output, output->wlr.output->current_mode)
      : lua_pushnil(L);
  } else if (streq(key, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua_mode_list->ref);
  } else if (streq(key, "scale")) {
    lua_pushnumber(L, output->wlr.output->scale);
  } else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static bool
catnip_lua_output__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_output* output = lua_resource->data;

  if (streq(key, "x")) {
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
    struct wlr_output_mode* mode = catnip_lua_resource_checkname(L, 3, "mode");
    struct wlr_output_state state = {0};

    wlr_output_state_set_mode(&state, mode);
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
  } else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct catnip_lua_resource*
catnip_lua_output_create(lua_State* L, struct catnip_output* output)
{
  struct catnip_lua_resource* lua_resource = catnip_lua_resource_create(L);
  output->lua_resource = lua_resource;
  output->lua_mode_list = catnip_lua_resource_list_create(L);

  struct wlr_output_mode* output_mode = NULL;
  wl_list_for_each(output_mode, &output->wlr.output->modes, link)
  {
    wl_list_insert(
      &output->lua_mode_list->head,
      &catnip_lua_output_mode_create(L, output_mode)->link
    );
  }

  lua_resource->data = output;
  lua_resource->name = "output";
  lua_resource->__index = catnip_lua_output__index;
  lua_resource->__newindex = catnip_lua_output__newindex;

  wl_list_insert(&catnip_lua_outputs->outputs, &lua_resource->link);

  catnip_lua_output_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
catnip_lua_output_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  struct catnip_output* output = lua_resource->data;

  catnip_lua_output_publish(L, lua_resource, "destroy", 0);

  struct catnip_lua_resource* lua_output_mode = NULL;
  wl_list_for_each(lua_output_mode, &output->lua_mode_list->head, link)
  {
    catnip_lua_output_mode_destroy(L, lua_output_mode);
  }

  catnip_lua_resource_list_destroy(L, output->lua_mode_list);
  catnip_lua_resource_destroy(L, lua_resource);
}

void
catnip_lua_output_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
)
{
  catnip_lua_resource_publish(L, lua_resource, event, nargs);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_resource->ref);
  lua_insert(L, -1 - nargs);

  catnip_lua_events_publish(
    L,
    catnip_lua_outputs->subscriptions,
    event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
}
