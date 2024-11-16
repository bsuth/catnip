#include "lua_widget_svg.h"
#include "extensions/lua.h"
#include "extensions/string.h"
#include "log.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static bool
is_svg_filename(const char* document)
{
  size_t len = strlen(document);
  return len > 3 && document[len - 4] == '.' && document[len - 3] == 's'
    && document[len - 2] == 'v' && document[len - 1] == 'g';
}

static void
catnip_lua_widget_svg_reload(lua_State* L, struct catnip_lua_widget_svg* svg)
{
  if (svg->rsvg != NULL) {
    g_object_unref(svg->rsvg);
    svg->rsvg = NULL;
  }

  GError* error = NULL;
  svg->rsvg = is_svg_filename(svg->styles.document)
    ? rsvg_handle_new_from_file(svg->styles.document, &error)
    : rsvg_handle_new_from_data(
        (const guint8*) svg,
        strlen(svg->styles.document),
        &error
      );

  if (error != NULL) {
    catnip_log_error("failed to load svg (%s)", error->message);
    g_error_free(error);
    return;
  }

  double intrinsic_width;
  double intrinsic_height;

  rsvg_handle_get_intrinsic_size_in_pixels(
    svg->rsvg,
    &intrinsic_width,
    &intrinsic_height
  );

  svg->aspect_ratio = intrinsic_width / intrinsic_height;

  if (svg->styles.stylesheet != NULL) {
    rsvg_handle_set_stylesheet(
      svg->rsvg,
      (const guint8*) svg->styles.stylesheet,
      strlen(svg->styles.stylesheet),
      &error
    );

    if (error != NULL) {
      catnip_log_error("failed to load svg styles (%s)", error->message);
      g_error_free(error);
    }
  }
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_svg_set_document(
  lua_State* L,
  struct catnip_lua_widget_svg* svg,
  int idx
)
{
  svg->styles.document = luaL_checkstring(L, idx);
  luaL_unref(L, LUA_REGISTRYINDEX, svg->styles.document_ref);
  lua_pushvalue(L, idx);
  svg->styles.document_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

static void
catnip_lua_widget_svg_set_stylesheet(
  lua_State* L,
  struct catnip_lua_widget_svg* svg,
  int idx
)
{
  if (lua_type(L, idx) == LUA_TNIL) {
    svg->styles.stylesheet = NULL;
    luaL_unref(L, LUA_REGISTRYINDEX, svg->styles.stylesheet_ref);
    svg->styles.stylesheet_ref = LUA_NOREF;
  } else {
    svg->styles.stylesheet = luaL_checkstring(L, idx);
    luaL_unref(L, LUA_REGISTRYINDEX, svg->styles.stylesheet_ref);
    lua_pushvalue(L, idx);
    svg->styles.stylesheet_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_svg__index(lua_State* L)
{
  struct catnip_lua_widget_svg* svg = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "type")) {
    lua_pushliteral(L, "svg");
  } else if (streq(key, "document")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, svg->styles.document_ref);
  } else if (streq(key, "stylesheet")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, svg->styles.stylesheet_ref);
  } else if (streq(key, "aspect_ratio")) {
    lua_pushnumber(L, svg->aspect_ratio);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_svg__newindex(lua_State* L)
{
  struct catnip_lua_widget_svg* svg = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  } else if (streq(key, "document")) {
    catnip_lua_widget_svg_set_document(L, svg, 3);
    catnip_lua_widget_svg_reload(L, svg);
  } else if (streq(key, "stylesheet")) {
    catnip_lua_widget_svg_set_stylesheet(L, svg, 3);
    catnip_lua_widget_svg_reload(L, svg);
  }

  return 0;
}

static int
catnip_lua_widget_svg__gc(lua_State* L)
{
  struct catnip_lua_widget_svg* svg = lua_touserdata(L, 1);

  if (svg->rsvg != NULL) {
    g_object_unref(svg->rsvg);
  }

  luaL_unref(L, LUA_REGISTRYINDEX, svg->styles.document_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, svg->styles.stylesheet_ref);

  catnip_lua_widget_base_cleanup(L, &svg->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_svg_mt[] = {
  {"__index", catnip_lua_widget_svg__index},
  {"__newindex", catnip_lua_widget_svg__newindex},
  {"__gc", catnip_lua_widget_svg__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_svg_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.svg");
  luaL_setfuncs(L, catnip_lua_widget_svg_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_svg(lua_State* L)
{
  struct catnip_lua_widget_svg* svg =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_svg));
  luaL_setmetatable(L, "catnip.widget.svg");

  catnip_lua_widget_base_setup(L, &svg->base);
  svg->base.type = CATNIP_LUA_WIDGET_SVG;

  svg->rsvg = NULL;

  svg->styles.document = "";
  lua_pushliteral(L, "");
  svg->styles.document_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  svg->styles.stylesheet = NULL;
  svg->styles.stylesheet_ref = LUA_NOREF;

  if (lua_type(L, 1) == LUA_TTABLE) {
    if (lua_hasstringfield(L, 1, "document")) {
      catnip_lua_widget_svg_set_document(L, svg, -1);
      lua_pop(L, 1);
    }

    if (lua_hasstringfield(L, 1, "stylesheet")) {
      catnip_lua_widget_svg_set_stylesheet(L, svg, -1);
      lua_pop(L, 1);
    }

    catnip_lua_widget_svg_reload(L, svg);
  }

  return 1;
}

void
catnip_lua_widget_svg_draw(
  lua_State* L,
  struct catnip_lua_widget_svg* svg,
  cairo_t* cr
)
{
  if (svg->rsvg == NULL) {
    return;
  }

  RsvgRectangle viewport = {
    .x = svg->base.bounding_box.x,
    .y = svg->base.bounding_box.y,
    .width = svg->base.bounding_box.width,
    .height = svg->base.bounding_box.height,
  };

  rsvg_handle_render_document(svg->rsvg, cr, &viewport, NULL);
}
