#include "lua_widget_text.h"
#include "extensions/string.h"
#include <lauxlib.h>
#include <pango/pangocairo.h>

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_text_set_text(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  int idx
)
{
  text->styles.text = luaL_checkstring(L, idx);
  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.text_ref);
  lua_pushvalue(L, idx);
  text->styles.text_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  pango_layout_set_text(text->layout, text->styles.text, -1);
}

static void
catnip_lua_widget_text_set_font(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  int idx
)
{
  if (lua_type(L, idx) == LUA_TNIL) {
    text->styles.font = NULL;
    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.font_ref);
    text->styles.font_ref = LUA_NOREF;
  } else {
    text->styles.font = luaL_checkstring(L, idx);
    lua_pushvalue(L, idx);
    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.font_ref);
    text->styles.font_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

static void
catnip_lua_widget_text_set_halign(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  int idx
)
{
  if (lua_type(L, idx) == LUA_TNIL) {
    text->styles.halign = -1;
    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.halign_ref);
    text->styles.halign_ref = LUA_NOREF;
  } else {
    const char* halign = luaL_checkstring(L, idx);

    if (streq(halign, "left")) {
      text->styles.halign = PANGO_ALIGN_LEFT;
    } else if (streq(halign, "center")) {
      text->styles.halign = PANGO_ALIGN_CENTER;
    } else if (streq(halign, "right")) {
      text->styles.halign = PANGO_ALIGN_RIGHT;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.halign_ref);
    lua_pushvalue(L, idx);
    text->styles.halign_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

static void
catnip_lua_widget_text_set_valign(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  int idx
)
{
  if (lua_type(L, idx) == LUA_TNIL) {
    text->styles.valign = -1;
    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.valign_ref);
    text->styles.valign_ref = LUA_NOREF;
  } else {
    const char* valign = luaL_checkstring(L, idx);

    if (streq(valign, "top")) {
      text->styles.valign = PANGO_ALIGN_LEFT;
    } else if (streq(valign, "center")) {
      text->styles.valign = PANGO_ALIGN_CENTER;
    } else if (streq(valign, "bottom")) {
      text->styles.valign = PANGO_ALIGN_RIGHT;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.valign_ref);
    lua_pushvalue(L, idx);
    text->styles.valign_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

static void
catnip_lua_widget_text_set_ellipsis(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  int idx
)
{
  if (lua_type(L, idx) == LUA_TNIL) {
    text->styles.ellipsis = -1;
    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.ellipsis_ref);
    text->styles.ellipsis_ref = LUA_NOREF;
  } else {
    const char* ellipsis = luaL_checkstring(L, idx);

    if (streq(ellipsis, "start")) {
      text->styles.ellipsis = PANGO_ELLIPSIZE_START;
    } else if (streq(ellipsis, "middle")) {
      text->styles.ellipsis = PANGO_ELLIPSIZE_MIDDLE;
    } else if (streq(ellipsis, "end")) {
      text->styles.ellipsis = PANGO_ELLIPSIZE_END;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.ellipsis_ref);
    lua_pushvalue(L, idx);
    text->styles.ellipsis_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

static void
catnip_lua_widget_text_set_wrap(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  int idx
)
{
  if (lua_type(L, idx) == LUA_TNIL) {
    text->styles.wrap = -1;
    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.wrap_ref);
    text->styles.wrap_ref = LUA_NOREF;
  } else {
    const char* wrap = luaL_checkstring(L, idx);

    if (streq(wrap, "char")) {
      text->styles.wrap = PANGO_WRAP_CHAR;
    } else if (streq(wrap, "word")) {
      text->styles.wrap = PANGO_WRAP_WORD;
    } else if (streq(wrap, "auto")) {
      text->styles.wrap = PANGO_WRAP_WORD_CHAR;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, text->styles.wrap_ref);
    lua_pushvalue(L, idx);
    text->styles.wrap_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }
}

// -----------------------------------------------------------------------------
// Refresh
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_refresh_attributes(
  lua_State* L,
  struct catnip_lua_widget_text* text
)
{
  if (text->attributes != NULL) {
    pango_attr_list_unref(text->attributes);
  }

  text->attributes = pango_attr_list_new();

  if (text->styles.size != -1) {
    pango_attr_list_insert(
      text->attributes,
      pango_attr_size_new_absolute(PANGO_SCALE * text->styles.size)
    );
  }

  if (text->styles.weight != -1) {
    pango_attr_list_insert(
      text->attributes,
      pango_attr_weight_new(text->styles.weight)
    );
  }

  if (text->styles.italic) {
    pango_attr_list_insert(
      text->attributes,
      pango_attr_style_new(PANGO_STYLE_ITALIC)
    );
  }

  if (text->styles.color != -1) {
    const guint8 red8 = ((text->styles.color & 0xff0000) >> 16);
    const guint8 green8 = ((text->styles.color & 0x00ff00) >> 8);
    const guint8 blue8 = (text->styles.color & 0x0000ff);

    pango_attr_list_insert(
      text->attributes,
      pango_attr_foreground_new(
        ((guint16) red8 << 8) | red8,
        ((guint16) green8 << 8) | green8,
        ((guint16) blue8 << 8) | blue8
      )
    );
  }

  if (text->styles.opacity != -1) {
    pango_attr_list_insert(
      text->attributes,
      pango_attr_foreground_alpha_new(0xffff * text->styles.opacity)
    );
  }

  if (text->styles.font != NULL) {
    pango_attr_list_insert(
      text->attributes,
      pango_attr_family_new(text->styles.font)
    );
  }

  text->refresh_attributes = false;
  pango_layout_set_attributes(text->layout, text->attributes);
}

// -----------------------------------------------------------------------------
// Lua Methods
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_text_lua_measure(lua_State* L)
{
  struct catnip_lua_widget_text* text =
    luaL_checkudata(L, 1, "catnip.widget.text");

  int layout_width = pango_layout_get_width(text->layout);
  int layout_height = pango_layout_get_height(text->layout);

  pango_layout_set_width(text->layout, -1);
  pango_layout_set_height(text->layout, -1);

  int intrinsic_width = -1;
  int intrinsic_height = -1;

  pango_layout_get_pixel_size(
    text->layout,
    &intrinsic_width,
    &intrinsic_height
  );

  pango_layout_set_width(text->layout, layout_width);
  pango_layout_set_height(text->layout, layout_height);

  lua_pushnumber(L, intrinsic_width);
  lua_pushnumber(L, intrinsic_height);

  return 2;
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_text__index(lua_State* L)
{
  struct catnip_lua_widget_text* text = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "type")) {
    lua_pushliteral(L, "text");
  } else if (streq(key, "text")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, text->styles.text_ref);
  } else if (streq(key, "size")) {
    lua_pushnumber(L, text->styles.size);
  } else if (streq(key, "weight")) {
    lua_pushnumber(L, text->styles.weight);
  } else if (streq(key, "italic")) {
    lua_pushboolean(L, text->styles.italic);
  } else if (streq(key, "color")) {
    lua_pushnumber(L, text->styles.color);
  } else if (streq(key, "opacity")) {
    lua_pushnumber(L, text->styles.opacity);
  } else if (streq(key, "font")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, text->styles.font_ref);
  } else if (streq(key, "halign")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, text->styles.halign_ref);
  } else if (streq(key, "valign")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, text->styles.valign_ref);
  } else if (streq(key, "ellipsis")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, text->styles.ellipsis_ref);
  } else if (streq(key, "wrap")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, text->styles.wrap_ref);
  } else if (streq(key, "measure")) {
    lua_pushcfunction(L, catnip_lua_widget_text_lua_measure);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_text__newindex(lua_State* L)
{
  struct catnip_lua_widget_text* text = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  } else if (streq(key, "text")) {
    catnip_lua_widget_text_set_text(L, text, 3);
    catnip_lua_widget_base_request_layout(L, &text->base);
  } else if (streq(key, "size")) {
    text->styles.size = luaL_checkinteger(L, 3);
    text->refresh_attributes = true;
    catnip_lua_widget_base_request_layout(L, &text->base);
  } else if (streq(key, "weight")) {
    text->styles.weight = luaL_checkinteger(L, 3);
    text->refresh_attributes = true;
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "italic")) {
    text->styles.italic = lua_toboolean(L, 3);
    text->refresh_attributes = true;
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "color")) {
    text->styles.color = luaL_checkinteger(L, 3);
    text->refresh_attributes = true;
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "opacity")) {
    text->styles.opacity = luaL_checknumber(L, 3);
    text->refresh_attributes = true;
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "font")) {
    catnip_lua_widget_text_set_font(L, text, 3);
    text->refresh_attributes = true;
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "halign")) {
    catnip_lua_widget_text_set_halign(L, text, 3);
    pango_layout_set_alignment(text->layout, text->styles.halign);
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "valign")) {
    catnip_lua_widget_text_set_valign(L, text, 3);
    catnip_lua_widget_base_request_draw(L, &text->base);
  } else if (streq(key, "ellipsis")) {
    catnip_lua_widget_text_set_ellipsis(L, text, 3);
    pango_layout_set_ellipsize(text->layout, text->styles.ellipsis);
    catnip_lua_widget_base_request_layout(L, &text->base);
  } else if (streq(key, "wrap")) {
    catnip_lua_widget_text_set_wrap(L, text, 3);
    pango_layout_set_wrap(text->layout, text->styles.wrap);
    catnip_lua_widget_base_request_layout(L, &text->base);
  }

  return 0;
}

static int
catnip_lua_widget_text__gc(lua_State* L)
{
  struct catnip_lua_widget_text* text = lua_touserdata(L, 1);

  if (text->attributes != NULL) {
    pango_attr_list_unref(text->attributes);
  }

  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.text_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.font_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.halign_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.valign_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.ellipsis_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, text->styles.wrap_ref);

  free(text->layout);
  g_object_unref(text->context);

  catnip_lua_widget_base_cleanup(L, &text->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_text_mt[] = {
  {"__index", catnip_lua_widget_text__index},
  {"__newindex", catnip_lua_widget_text__newindex},
  {"__gc", catnip_lua_widget_text__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_text_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.text");
  luaL_setfuncs(L, catnip_lua_widget_text_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_text(lua_State* L)
{
  struct catnip_lua_widget_text* text =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_text));
  luaL_setmetatable(L, "catnip.widget.text");

  catnip_lua_widget_base_setup(L, &text->base);
  text->base.type = CATNIP_LUA_WIDGET_TEXT;

  PangoFontMap* fontmap = pango_cairo_font_map_get_default();
  text->context = pango_font_map_create_context(fontmap);
  text->layout = pango_layout_new(text->context);
  text->attributes = NULL;
  text->refresh_attributes = false;

  text->styles.text = "";
  lua_pushliteral(L, "");
  text->styles.text_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  text->styles.size = -1;
  text->styles.weight = -1;
  text->styles.italic = false;
  text->styles.color = -1;
  text->styles.opacity = -1;
  text->styles.font = NULL;
  text->styles.font_ref = LUA_NOREF;
  text->styles.halign = -1;
  text->styles.halign_ref = LUA_NOREF;
  text->styles.valign = -1;
  text->styles.valign_ref = LUA_NOREF;
  text->styles.ellipsis = -1;
  text->styles.ellipsis_ref = LUA_NOREF;
  text->styles.wrap = -1;
  text->styles.wrap_ref = LUA_NOREF;

  if (lua_type(L, 1) == LUA_TTABLE) {
    if (lua_hasstringfield(L, 1, "text")) {
      catnip_lua_widget_text_set_text(L, text, -1);
      lua_pop(L, 1);
    }

    if (lua_hasnumberfield(L, 1, "size")) {
      text->styles.size = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 1, "weight")) {
      text->styles.weight = lua_popinteger(L);
    }

    if (lua_hasbooleanfield(L, 1, "italic")) {
      text->styles.italic = lua_popboolean(L);
    }

    if (lua_hasnumberfield(L, 1, "color")) {
      text->styles.color = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 1, "opacity")) {
      text->styles.opacity = lua_popnumber(L);
    }

    if (lua_hasstringfield(L, 1, "font")) {
      catnip_lua_widget_text_set_font(L, text, -1);
      lua_pop(L, 1);
    }

    if (lua_hasstringfield(L, 1, "halign")) {
      catnip_lua_widget_text_set_halign(L, text, -1);
      lua_pop(L, 1);
    }

    if (lua_hasstringfield(L, 1, "valign")) {
      catnip_lua_widget_text_set_valign(L, text, -1);
      lua_pop(L, 1);
    }

    if (lua_hasstringfield(L, 1, "ellipsis")) {
      catnip_lua_widget_text_set_ellipsis(L, text, -1);
      lua_pop(L, 1);
    }

    if (lua_hasstringfield(L, 1, "wrap")) {
      catnip_lua_widget_text_set_wrap(L, text, -1);
      lua_pop(L, 1);
    }

    text->refresh_attributes = true;
  }

  return 1;
}

void
catnip_lua_widget_text_draw(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  cairo_t* cr
)
{
  if (text->refresh_attributes) {
    catnip_lua_widget_refresh_attributes(L, text);
  }

  pango_layout_set_width(
    text->layout,
    PANGO_SCALE * text->base.bounding_box.width
  );
  pango_layout_set_height(
    text->layout,
    PANGO_SCALE * text->base.bounding_box.height
  );

  int draw_x = text->base.bounding_box.x;
  int draw_y = text->base.bounding_box.y;

  bool has_trivial_valign =
    text->styles.valign == -1 || text->styles.valign == PANGO_ALIGN_LEFT;

  if (!has_trivial_valign) {
    int intrinsic_height = -1;

    pango_layout_set_height(text->layout, -1);
    pango_layout_get_pixel_size(text->layout, NULL, &intrinsic_height);
    pango_layout_set_height(
      text->layout,
      PANGO_SCALE * text->base.bounding_box.height
    );

    // If we have overflow w/ ellipsis, then the content will take up the whole
    // height available and there is nothing to align.
    bool has_active_ellipsis = text->styles.ellipsis != -1
      && intrinsic_height > text->base.bounding_box.height;

    if (!has_active_ellipsis) {
      if (text->styles.valign == PANGO_ALIGN_CENTER) {
        draw_y += (text->base.bounding_box.height - intrinsic_height) / 2;
      } else if (text->styles.valign == PANGO_ALIGN_RIGHT) {
        draw_y += text->base.bounding_box.height - intrinsic_height;
      }
    }
  }

  cairo_save(cr);
  cairo_move_to(cr, draw_x, draw_y);
  pango_cairo_show_layout(cr, text->layout);
  cairo_restore(cr);
}
