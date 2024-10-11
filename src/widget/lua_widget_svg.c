#include "lua_widget_svg.h"
#include "extensions/lua.h"
#include "extensions/string.h"
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

  if (svg->styles.document == NULL) {
    return;
  }

  GError* error = NULL;
  RsvgHandle* rsvg = is_svg_filename(svg->styles.document)
    ? rsvg_handle_new_from_file(svg->styles.document, &error)
    : rsvg_handle_new_from_data(
        (const guint8*) svg,
        strlen(svg->styles.document),
        &error
      );

  if (error != NULL) {
    lua_log_error(L, "failed to load svg: %s", error->message);
    g_error_free(error);
    return;
  }

  svg->rsvg = rsvg;
  svg->styles.document = strdup(svg->styles.document);

  rsvg_handle_get_intrinsic_size_in_pixels(
    svg->rsvg,
    &svg->intrinsic_width,
    &svg->intrinsic_height
  );

  if (svg->styles.stylesheet != NULL) {
    rsvg_handle_set_stylesheet(
      svg->rsvg,
      (const guint8*) svg->styles.stylesheet,
      strlen(svg->styles.stylesheet),
      &error
    );

    if (error != NULL) {
      lua_log_error(L, "failed to load svg styles: %s", error->message);
      g_error_free(error);
    }
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
  } else if (catnip_lua_widget_base__index(L, &svg->base, key)) {
    return 1;
  } else if (streq(key, "document")) {
    lua_pushstring(L, svg->styles.document);
  } else if (streq(key, "stylesheet")) {
    lua_pushstring(L, svg->styles.stylesheet);
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

  if (key == NULL || catnip_lua_widget_base__newindex(L, &svg->base, key)) {
    return 0;
  } else if (streq(key, "document")) {
    const char* new_document = lua_tostring(L, 3);

    if (svg->styles.document != NULL) {
      free(svg->styles.document);
    }

    svg->styles.document = new_document == NULL ? NULL : strdup(new_document);
    catnip_lua_widget_svg_reload(L, svg);
  } else if (streq(key, "stylesheet")) {
    const char* new_stylesheet = lua_tostring(L, 3);

    if (svg->styles.stylesheet != NULL) {
      free(svg->styles.stylesheet);
    }

    svg->styles.stylesheet =
      new_stylesheet == NULL ? NULL : strdup(new_stylesheet);
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

  if (svg->styles.document != NULL) {
    free(svg->styles.document);
  }

  if (svg->styles.stylesheet != NULL) {
    free(svg->styles.stylesheet);
  }

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
  svg->styles.document = NULL;
  svg->styles.stylesheet = NULL;

  if (lua_type(L, 1) == LUA_TTABLE) {
    if (lua_hasstringfield(L, 1, "document")) {
      svg->styles.document = strdup(lua_popstring(L));
    }

    if (lua_hasstringfield(L, 1, "stylesheet")) {
      svg->styles.stylesheet = strdup(lua_popstring(L));
    }

    catnip_lua_widget_svg_reload(L, svg);
  }

  return 1;
}

static void
catnip_lua_widget_svg_layout(lua_State* L, struct catnip_lua_widget_svg* svg)
{
  // TODO: intrinsic scaling
  // if (viewport.width != -1 && viewport.height == -1) {
  //   viewport.height *= viewport.width / svg->intrinsic_width;
  // } else if (viewport.width == -1 && viewport.height != -1) {
  //   viewport.width *= viewport.height / svg->intrinsic_height;
  // }
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
    .x = svg->base.computed.x,
    .y = svg->base.computed.y,
    .width = svg->base.computed.width,
    .height = svg->base.computed.height,
  };

  rsvg_handle_render_document(svg->rsvg, cr, &viewport, NULL);
}
