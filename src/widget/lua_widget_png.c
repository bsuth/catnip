#include "lua_widget_png.h"
#include "extensions/lua.h"
#include "extensions/string.h"
#include "log.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_png_set_path(
  lua_State* L,
  struct catnip_lua_widget_png* png,
  int idx
)
{
  png->styles.path = luaL_checkstring(L, idx);

  luaL_unref(L, LUA_REGISTRYINDEX, png->styles.path_ref);
  lua_pushvalue(L, idx);
  png->styles.path_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  png->surface = cairo_image_surface_create_from_png(png->styles.path);

  if (cairo_surface_status(png->surface) == CAIRO_STATUS_SUCCESS) {
    png->intrinsic_width = cairo_image_surface_get_width(png->surface);
    png->intrinsic_height = cairo_image_surface_get_height(png->surface);
  } else {
    png->surface = NULL;
    png->intrinsic_width = 0;
    png->intrinsic_height = 1;
    catnip_log_error("failed to load %s", png->styles.path);
  }
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_png__index(lua_State* L)
{
  struct catnip_lua_widget_png* png = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "type")) {
    lua_pushliteral(L, "png");
  } else if (streq(key, "path")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, png->styles.path_ref);
  } else if (streq(key, "aspect_ratio")) {
    lua_pushnumber(
      L,
      (double) png->intrinsic_width / (double) png->intrinsic_height
    );
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_png__newindex(lua_State* L)
{
  struct catnip_lua_widget_png* png = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  } else if (streq(key, "path")) {
    catnip_lua_widget_png_set_path(L, png, 3);
  }

  return 0;
}

static int
catnip_lua_widget_png__gc(lua_State* L)
{
  struct catnip_lua_widget_png* png = lua_touserdata(L, 1);

  luaL_unref(L, LUA_REGISTRYINDEX, png->styles.path_ref);

  if (png->surface != NULL) {
    cairo_surface_destroy(png->surface);
  }

  catnip_lua_widget_base_cleanup(L, &png->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_png_mt[] = {
  {"__index", catnip_lua_widget_png__index},
  {"__newindex", catnip_lua_widget_png__newindex},
  {"__gc", catnip_lua_widget_png__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_png_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.png");
  luaL_setfuncs(L, catnip_lua_widget_png_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_png(lua_State* L)
{
  struct catnip_lua_widget_png* png =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_png));
  luaL_setmetatable(L, "catnip.widget.png");

  catnip_lua_widget_base_setup(L, &png->base);
  png->base.type = CATNIP_LUA_WIDGET_PNG;

  png->surface = NULL;
  png->intrinsic_width = 0;
  png->intrinsic_height = 1;

  png->styles.path = "";
  lua_pushliteral(L, "");
  png->styles.path_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  if (lua_type(L, 1) == LUA_TTABLE) {
    if (lua_hasstringfield(L, 1, "path")) {
      catnip_lua_widget_png_set_path(L, png, -1);
      lua_pop(L, 1);
    }
  }

  return 1;
}

void
catnip_lua_widget_png_draw(
  lua_State* L,
  struct catnip_lua_widget_png* png,
  cairo_t* cr
)
{
  if (png->surface == NULL) {
    return;
  }

  cairo_save(cr);
  cairo_scale(
    cr,
    (double) png->base.bounding_box.width / png->intrinsic_width,
    (double) png->base.bounding_box.height / png->intrinsic_height
  );
  cairo_set_source_surface(
    cr,
    png->surface,
    png->base.bounding_box.x,
    png->base.bounding_box.y
  );
  cairo_paint(cr);
  cairo_restore(cr);
}
