#include "lua_output.h"
#include "lua_resource.h"
#include "output/lua_output_list.h"
#include "output/lua_output_mode_list.h"
#include "output/output_layout.h"
#include "output/output_methods.h"
#include "utils/string.h"
#include <lauxlib.h>

void
lua_catnip_output_push_mode(
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
lua_catnip_output__index(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_output* output = lua_resource->data;

  if (streq(key, "id")) {
    lua_pushnumber(L, output->id);
  } else if (streq(key, "x")) {
    int x = wlr_output_layout_get(catnip_output_layout, output->wlr_output)->x;
    lua_pushnumber(L, x);
  } else if (streq(key, "y")) {
    int y = wlr_output_layout_get(catnip_output_layout, output->wlr_output)->y;
    lua_pushnumber(L, y);
  } else if (streq(key, "width")) {
    output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? lua_pushnumber(L, output->wlr_output->pending.custom_mode.width)
      : output->wlr_output->pending.mode != NULL
      ? lua_pushnumber(L, output->wlr_output->pending.mode->width)
      : lua_pushnumber(L, output->wlr_output->width);
  } else if (streq(key, "height")) {
    output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? lua_pushnumber(L, output->wlr_output->pending.custom_mode.height)
      : output->wlr_output->pending.mode != NULL
      ? lua_pushnumber(L, output->wlr_output->pending.mode->height)
      : lua_pushnumber(L, output->wlr_output->height);
  } else if (streq(key, "refresh")) {
    output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? lua_pushnumber(L, output->wlr_output->pending.custom_mode.refresh)
      : output->wlr_output->pending.mode != NULL
      ? lua_pushnumber(L, output->wlr_output->pending.mode->refresh)
      : lua_pushnumber(L, output->wlr_output->refresh);
  } else if (streq(key, "mode")) {
    output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? lua_pushnil(L)
      : output->wlr_output->pending.mode != NULL
      ? lua_catnip_output_push_mode(L, output, output->wlr_output->pending.mode)
      : output->wlr_output->current_mode != NULL
      ? lua_catnip_output_push_mode(L, output, output->wlr_output->current_mode)
      : lua_pushnil(L);
  } else if (streq(key, "modes")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua_mode_list->ref);
  } else if (streq(key, "scale")) {
    lua_pushnumber(L, output->wlr_output->pending.scale);
  } else {
    return false;
  }

  return true;
}

static bool
lua_catnip_output__newindex(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
)
{
  struct catnip_output* output = lua_resource->data;

  if (streq(key, "x")) {
    wlr_output_layout_add(
      catnip_output_layout,
      output->wlr_output,
      luaL_checknumber(L, 3),
      wlr_output_layout_get(catnip_output_layout, output->wlr_output)->y
    );
  } else if (streq(key, "y")) {
    wlr_output_layout_add(
      catnip_output_layout,
      output->wlr_output,
      wlr_output_layout_get(catnip_output_layout, output->wlr_output)->x,
      luaL_checknumber(L, 3)
    );
  } else if (streq(key, "width")) {
    catnip_output_configure(output, luaL_checknumber(L, 3), -1, -1);
  } else if (streq(key, "height")) {
    catnip_output_configure(output, -1, luaL_checknumber(L, 3), -1);
  } else if (streq(key, "refresh")) {
    catnip_output_configure(output, -1, -1, luaL_checknumber(L, 3));
  } else if (streq(key, "mode")) {
    wlr_output_set_mode(
      output->wlr_output,
      lua_catnip_resource_checkname(L, 3, "mode")
    );
  } else if (streq(key, "scale")) {
    wlr_output_set_scale(output->wlr_output, luaL_checknumber(L, 3));
  } else {
    return false;
  }

  return true;
}

struct catnip_lua_resource*
lua_catnip_output_create(lua_State* L, struct catnip_output* output)
{
  struct catnip_lua_resource* lua_resource = lua_catnip_resource_create(L);
  output->lua_resource = lua_resource;
  output->lua_mode_list = lua_catnip_output_mode_list_create(L, output);

  lua_resource->data = output;
  lua_resource->name = "output";
  lua_resource->__index = lua_catnip_output__index;
  lua_resource->__newindex = lua_catnip_output__newindex;

  wl_list_insert(&lua_catnip_output_list->head, &lua_resource->link);

  lua_catnip_resource_publish(L, lua_resource, "create", 0);

  return lua_resource;
}

void
lua_catnip_output_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
)
{
  struct catnip_output* output = lua_resource->data;
  lua_catnip_resource_publish(L, lua_resource, "destroy", 0);
  lua_catnip_resource_list_destroy(L, output->lua_mode_list);
  lua_catnip_resource_destroy(L, lua_resource);
}
