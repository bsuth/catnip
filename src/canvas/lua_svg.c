#include "lua_svg.h"
#include "canvas/canvas_methods.h"
#include "utils/lua.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

static bool
is_svg_filename(const char* filename)
{
  size_t len = strlen(filename);
  return len > 3 && filename[len - 4] == '.' && filename[len - 3] == 's'
    && filename[len - 2] == 'v' && filename[len - 1] == 'g';
}

static void
lua_catnip_svg_reload(lua_State* L, struct lua_catnip_svg* lua_svg)
{
  GError* error = NULL;

  lua_svg->width = 0;
  lua_svg->height = 0;

  lua_svg->rsvg = is_svg_filename(lua_svg->data)
    ? rsvg_handle_new_from_file(lua_svg->data, &error)
    : rsvg_handle_new_from_data(
      (const guint8*) lua_svg,
      strlen(lua_svg->data),
      &error
    );

  if (error != NULL) {
    lua_log_error(L, "bad svg (%s)", error->message);
    g_error_free(error);
    return;
  }

  rsvg_handle_get_intrinsic_size_in_pixels(
    lua_svg->rsvg,
    &lua_svg->width,
    &lua_svg->height
  );
}

static void
lua_catnip_svg_apply_styles(
  lua_State* L,
  struct lua_catnip_svg* lua_svg,
  const char* stylesheet
)
{
  if (lua_svg->rsvg == NULL) {
    return;
  }

  GError* error = NULL;

  rsvg_handle_set_stylesheet(
    lua_svg->rsvg,
    (const guint8*) stylesheet,
    strlen(stylesheet),
    &error
  );

  if (error != NULL) {
    lua_log_error(L, "bad stylesheet (%s)", error->message);
    g_error_free(error);
  }
}

static int
lua_catnip_svg_method_apply_styles(lua_State* L)
{
  lua_catnip_svg_apply_styles(
    L,
    luaL_checkudata(L, 1, "catnip.svg"),
    luaL_checkstring(L, 2)
  );

  return 0;
}

static int
lua_catnip_svg__index(lua_State* L)
{
  struct lua_catnip_svg* lua_svg = lua_touserdata(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "data")) {
    lua_pushstring(L, lua_svg->data);
  } else if (streq(key, "apply_styles")) {
    lua_pushcfunction(L, lua_catnip_svg_method_apply_styles);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_svg__newindex(lua_State* L)
{
  struct lua_catnip_svg* lua_svg = lua_touserdata(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_log_warning(
      L,
      "attempt to index resource with type '%s'",
      luaL_typename(L, 2)
    );
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "data")) {
    const char* data = luaL_checkstring(L, 3);
    free(lua_svg->data);
    lua_svg->data = strdup(data);
    lua_catnip_svg_reload(L, lua_svg);
  } else {
    lua_log_warning(L, "attempt to set unknown index '%s'", key);
  }

  return 0;
}

static int
lua_catnip_svg__gc(lua_State* L)
{
  struct lua_catnip_svg* lua_svg = lua_touserdata(L, 1);
  free(lua_svg->data);
  g_object_unref(lua_svg->rsvg);
  return 0;
}

static const struct luaL_Reg lua_catnip_svg_mt[] = {
  {"__index", lua_catnip_svg__index},
  {"__newindex", lua_catnip_svg__newindex},
  {"__gc", lua_catnip_svg__gc},
  {NULL, NULL}
};

int
lua_catnip_svg(lua_State* L)
{
  const char* data = luaL_checkstring(L, 1);

  struct lua_catnip_svg* lua_svg =
    lua_newuserdata(L, sizeof(struct lua_catnip_svg));
  luaL_setmetatable(L, "catnip.svg");

  lua_svg->data = strdup(data);
  lua_svg->rsvg = NULL;
  lua_catnip_svg_reload(L, lua_svg);

  if (lua_type(L, 2) == LUA_TTABLE) {
    if (lua_hasstringfield(L, 2, "stylesheet")) {
      lua_catnip_svg_apply_styles(L, lua_svg, lua_popstring(L));
    }
  }

  return 1;
}

void
lua_catnip_svg_render(
  struct lua_catnip_svg* lua_svg,
  struct catnip_canvas* canvas,
  double x,
  double y,
  double width,
  double height
)
{
  RsvgRectangle viewport = {
    .x = x,
    .y = y,
    .width = width == -1 ? lua_svg->width : width,
    .height = height == -1 ? lua_svg->height : height,
  };

  if (width != -1 && height == -1) {
    viewport.height *= width / lua_svg->width;
  } else if (width == -1 && height != -1) {
    viewport.width *= height / lua_svg->height;
  }

  rsvg_handle_render_document(lua_svg->rsvg, canvas->cr, &viewport, NULL);
  catnip_canvas_refresh(canvas);
}

void
lua_catnip_svg_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.svg");
  luaL_setfuncs(L, lua_catnip_svg_mt, 0);
  lua_pop(L, 1);
}
