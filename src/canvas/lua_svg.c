#include "lua_svg.h"
#include "canvas/canvas.h"
#include "lua_resource.h"
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

  lua_svg->rsvg = is_svg_filename(lua_svg->document)
    ? rsvg_handle_new_from_file(lua_svg->document, &error)
    : rsvg_handle_new_from_data(
        (const guint8*) lua_svg,
        strlen(lua_svg->document),
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

static int
lua_catnip_svg_apply(lua_State* L)
{
  struct lua_catnip_svg* lua_svg = luaL_checkudata(L, 1, "catnip.svg");
  const char* stylesheet = luaL_checkstring(L, 2);

  if (lua_svg->rsvg == NULL) {
    return 0;
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

  if (streq(key, "document")) {
    lua_pushstring(L, lua_svg->document);
  } else if (streq(key, "apply")) {
    lua_pushcfunction(L, lua_catnip_svg_apply);
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

  if (streq(key, "document")) {
    const char* document = luaL_checkstring(L, 3);
    free(lua_svg->document);
    lua_svg->document = strdup(document);
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
  free(lua_svg->document);
  g_object_unref(lua_svg->rsvg);
  return 0;
}

static const struct luaL_Reg lua_catnip_svg_mt[] = {
  {"__index", lua_catnip_svg__index},
  {"__newindex", lua_catnip_svg__newindex},
  {"__gc", lua_catnip_svg__gc},
  {NULL, NULL}
};

void
lua_catnip_svg_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.svg");
  luaL_setfuncs(L, lua_catnip_svg_mt, 0);
  lua_pop(L, 1);
}

int
lua_catnip_svg(lua_State* L)
{
  const char* document = luaL_checkstring(L, 1);

  struct lua_catnip_svg* lua_svg =
    lua_newuserdata(L, sizeof(struct lua_catnip_svg));
  luaL_setmetatable(L, "catnip.svg");

  lua_svg->document = strdup(document);
  lua_svg->rsvg = NULL;
  lua_catnip_svg_reload(L, lua_svg);

  return 1;
}

int
lua_catnip_canvas_svg(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  struct lua_catnip_svg* lua_svg = luaL_checkudata(L, 2, "catnip.svg");
  luaL_checktype(L, 3, LUA_TTABLE);

  double x = lua_hasfield(L, 3, "x") ? lua_popnumber(L) : 0;
  double y = lua_hasfield(L, 3, "y") ? lua_popnumber(L) : 0;
  double width = lua_hasfield(L, 3, "width") ? lua_popnumber(L) : -1;
  double height = lua_hasfield(L, 3, "height") ? lua_popnumber(L) : -1;

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

  return 0;
}
