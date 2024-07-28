#include "lua_canvas_text.h"
#include "canvas/canvas.h"
#include "lua_resource.h"
#include "utils/lua.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <pango/pango-layout.h>
#include <pango/pangocairo.h>

int
lua_catnip_canvas_text(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");

  PangoLayout* layout = pango_cairo_create_layout(canvas->cr);
  pango_layout_set_text(layout, luaL_checkstring(L, 2), -1);

  luaL_checktype(L, 3, LUA_TTABLE);

  PangoAttrList* attributes = pango_attr_list_new();
  pango_layout_set_attributes(layout, attributes);

  int x = lua_hasnumberfield(L, 3, "x") ? lua_popnumber(L) : 0;
  int y = lua_hasnumberfield(L, 3, "y") ? lua_popnumber(L) : 0;
  int height = -1;

  if (lua_hasnumberfield(L, 3, "width")) {
    pango_layout_set_width(layout, PANGO_SCALE * lua_popnumber(L));
  }

  if (lua_hasnumberfield(L, 3, "height")) {
    height = lua_popnumber(L);
    pango_layout_set_height(layout, PANGO_SCALE * height);
  }

  if (lua_hasnumberfield(L, 3, "font")) {
    pango_attr_list_insert(attributes, pango_attr_family_new(lua_popstring(L)));
  }

  if (lua_hasnumberfield(L, 3, "size")) {
    pango_attr_list_insert(
      attributes,
      pango_attr_size_new_absolute(PANGO_SCALE * lua_popinteger(L))
    );
  }

  if (lua_hasnumberfield(L, 3, "weight")) {
    pango_attr_list_insert(
      attributes,
      pango_attr_weight_new(lua_popinteger(L))
    );
  }

  if (lua_hasnumberfield(L, 3, "italic")) {
    pango_attr_list_insert(
      attributes,
      pango_attr_style_new(
        lua_popboolean(L) ? PANGO_STYLE_NORMAL : PANGO_STYLE_ITALIC
      )
    );
  }

  if (lua_hasnumberfield(L, 3, "color")) {
    int color = lua_popinteger(L);

    const guint8 red8 = ((color & 0xff0000) >> 16);
    const guint8 green8 = ((color & 0x00ff00) >> 8);
    const guint8 blue8 = (color & 0x0000ff);

    pango_attr_list_insert(
      attributes,
      pango_attr_foreground_new(
        ((guint16) red8 << 8) | red8,
        ((guint16) green8 << 8) | green8,
        ((guint16) blue8 << 8) | blue8
      )
    );
  }

  if (lua_hasnumberfield(L, 3, "opacity")) {
    pango_attr_list_insert(
      attributes,
      pango_attr_foreground_alpha_new(0xffff * lua_popnumber(L))
    );
  }

  if (lua_hasstringfield(L, 3, "align")) {
    const char* alignment = lua_popstring(L);

    if (streq(alignment, "left")) {
      pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    } else if (streq(alignment, "center")) {
      pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
    } else if (streq(alignment, "right")) {
      pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
    }
  }

  if (lua_hasfield(L, 3, "ellipsis")) {
    int ellipsis_type = lua_type(L, -1);

    if (ellipsis_type == LUA_TBOOLEAN) {
      pango_layout_set_ellipsize(
        layout,
        lua_toboolean(L, -1) ? PANGO_ELLIPSIZE_END : PANGO_ELLIPSIZE_NONE
      );
    } else if (ellipsis_type == LUA_TSTRING) {
      const char* ellipsis = lua_tostring(L, -1);

      if (streq(ellipsis, "start")) {
        pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_START);
      } else if (streq(ellipsis, "middle")) {
        pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_MIDDLE);
      } else if (streq(ellipsis, "end")) {
        pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
      }
    }

    lua_pop(L, 1);
  }

  if (lua_hasstringfield(L, 3, "wrap")) {
    int wrap_type = lua_type(L, -1);

    if (wrap_type == LUA_TBOOLEAN) {
      if (lua_toboolean(L, -1)) {
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
      }
    } else if (wrap_type == LUA_TSTRING) {
      const char* wrap = lua_tostring(L, -1);

      if (streq(wrap, "char")) {
        pango_layout_set_wrap(layout, PANGO_WRAP_CHAR);
      } else if (streq(wrap, "word")) {
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
      } else if (streq(wrap, "auto")) {
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
      }
    }

    lua_pop(L, 1);
  }

  // NOTE: Only get the pixel size after we are finished setting the layout.
  int layout_width = -1;
  int layout_height = -1;
  pango_layout_get_pixel_size(layout, &layout_width, &layout_height);

  if (height != -1 && lua_hasstringfield(L, 3, "valign")) {
    const char* alignment = lua_popstring(L);

    if (streq(alignment, "center")) {
      y += (height - layout_height) / 2;
    } else if (streq(alignment, "bottom")) {
      y += height - layout_height;
    }
  }

  if (!lua_hasbooleanfield(L, 3, "render") || lua_popboolean(L)) {
    cairo_save(canvas->cr);
    cairo_move_to(canvas->cr, x, y);
    pango_cairo_show_layout(canvas->cr, layout);
    cairo_restore(canvas->cr);
    catnip_canvas_refresh(canvas);
  }

  pango_attr_list_unref(attributes);

  lua_pushnumber(L, layout_width);
  lua_pushnumber(L, layout_height);

  return 2;
}
