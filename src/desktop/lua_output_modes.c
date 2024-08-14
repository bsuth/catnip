#include "lua_output_modes.h"
#include "desktop/lua_output_mode.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __call
// -----------------------------------------------------------------------------

static int
catnip_lua_output_modes__call(lua_State* L)
{
  struct catnip_lua_output_modes* lua_output_modes = lua_touserdata(L, 1);

  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? lua_output_modes->modes.next
    : ((struct catnip_lua_output_mode*) lua_touserdata(L, 3))->link.next;

  if (link == &lua_output_modes->modes) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_lua_output_mode* lua_output_mode =
    wl_container_of(link, lua_output_mode, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_output_mode->ref);
  return 1;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_output_modes_mt[] = {
  {"__call", catnip_lua_output_modes__call},
  {NULL, NULL}
};

void
catnip_lua_output_modes_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.output.modes");
  luaL_setfuncs(L, catnip_lua_output_modes_mt, 0);
  lua_pop(L, 1);
}

struct catnip_lua_output_modes*
catnip_lua_output_modes_create(lua_State* L, struct catnip_output* output)
{
  struct catnip_lua_output_modes* lua_output_modes =
    lua_newuserdata(L, sizeof(struct catnip_lua_output_modes));
  luaL_setmetatable(L, "catnip.output.modes");

  lua_output_modes->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  wl_list_init(&lua_output_modes->modes);

  struct wlr_output_mode* wlr_output_mode = NULL;
  wl_list_for_each(wlr_output_mode, &output->wlr.output->modes, link)
  {
    wl_list_insert(
      &lua_output_modes->modes,
      &catnip_lua_output_mode_create(L, wlr_output_mode)->link
    );
  }

  return lua_output_modes;
}

void
catnip_lua_output_modes_destroy(
  lua_State* L,
  struct catnip_lua_output_modes* lua_output_modes
)
{
  struct catnip_lua_output_mode* lua_output_mode = NULL;
  wl_list_for_each(lua_output_mode, &lua_output_modes->modes, link)
  {
    catnip_lua_output_mode_destroy(L, lua_output_mode);
  }

  luaL_unref(L, LUA_REGISTRYINDEX, lua_output_modes->ref);
}
