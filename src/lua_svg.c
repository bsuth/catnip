#include "lua_svg.h"
#include "utils/log.h"
#include <lauxlib.h>
#include <librsvg/rsvg.h>
#include <stdbool.h>

static bool
is_svg_filename(const char* filename)
{
  size_t len = strlen(filename);
  return (
    len > 3 && filename[len - 4] == '.' && filename[len - 3] == 's'
    && filename[len - 2] == 'v' && filename[len - 1] == 'g'
  );
}

static int
lua_catnip_svg__gc(lua_State* L)
{
  RsvgHandle** lua_svg = lua_touserdata(L, 1);
  g_object_unref(*lua_svg);
  return 0;
}

static const struct luaL_Reg lua_catnip_svg_metatable[] = {
  {"__gc", lua_catnip_svg__gc},
  {NULL, NULL}
};

void
lua_catnip_svg_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.svg");
  luaL_setfuncs(L, lua_catnip_svg_metatable, 0);
  lua_pop(L, 1);
}

int
lua_catnip_svg_create(lua_State* L)
{
  const char* data = luaL_checkstring(L, 1);

  GError* error = NULL;
  RsvgHandle* rsvg =
    is_svg_filename(data)
      ? rsvg_handle_new_from_file(data, &error)
      : rsvg_handle_new_from_data((const guint8*) data, strlen(data), &error);

  if (error != NULL) {
    log_error("failed to load svg (%s)", error->message);
    g_error_free(error);
    lua_pushnil(L);
    return 1;
  }

  RsvgHandle** lua_svg = lua_newuserdata(L, sizeof(RsvgHandle*));
  luaL_setmetatable(L, "catnip.svg");
  *lua_svg = rsvg;

  if (lua_type(L, 2) == LUA_TSTRING) {
    const char* stylesheet = lua_tostring(L, 2);

    rsvg_handle_set_stylesheet(
      rsvg,
      (const guint8*) stylesheet,
      strlen(stylesheet),
      &error
    );

    if (error != NULL) {
      log_error("failed to load stylesheet (%s)", error->message);
      g_error_free(error);
    }
  }

  // TODO: need to set dpi?
  // rsvg_handle_set_dpi(svg, 0);

  return 1;
}
