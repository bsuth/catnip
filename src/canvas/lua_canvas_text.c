#include "lua_canvas_text.h"
#include "canvas/canvas_methods.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>
#include <pango/pango-layout.h>
#include <pango/pangocairo.h>

struct catnip_canvas_text {
  const char* text;

  int x;
  int y;
  int width;
  int height;

  PangoAttrList* attributes;
  PangoAlignment alignment;
  PangoEllipsizeMode ellipsize;
  PangoWrapMode wrap;
};

static void
catnip_canvas_text_draw(
  struct catnip_canvas* canvas,
  struct catnip_canvas_text* text
)
{
  PangoLayout* layout = pango_cairo_create_layout(canvas->cr);

  pango_layout_set_text(layout, text->text, -1);
  pango_layout_set_width(layout, text->width);
  pango_layout_set_height(layout, text->height);
  pango_layout_set_attributes(layout, text->attributes);
  pango_layout_set_alignment(layout, text->alignment);
  pango_layout_set_ellipsize(layout, text->ellipsize);
  pango_layout_set_wrap(layout, text->wrap);

  cairo_save(canvas->cr);
  cairo_move_to(canvas->cr, text->x, text->y);
  pango_cairo_show_layout(canvas->cr, layout);
  cairo_restore(canvas->cr);

  catnip_canvas_refresh(canvas);
}

int
lua_catnip_canvas_text(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *lua_canvas;

  struct catnip_canvas_text text = {
    .text = luaL_checkstring(L, 2),
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
    .attributes = pango_attr_list_new(),
    .alignment = PANGO_ALIGN_LEFT,
    .ellipsize = PANGO_ELLIPSIZE_NONE,
    .wrap = PANGO_WRAP_WORD_CHAR,
  };

  if (lua_type(L, 3) == LUA_TTABLE) {
    if (lua_hasnumberfield(L, 3, "x")) {
      text.x = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "y")) {
      text.y = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "width")) {
      text.width = PANGO_SCALE * lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "height")) {
      text.height = PANGO_SCALE * lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "font")) {
      pango_attr_list_insert(
        text.attributes,
        pango_attr_family_new(lua_popstring(L))
      );
    }

    if (lua_hasnumberfield(L, 3, "size")) {
      pango_attr_list_insert(
        text.attributes,
        pango_attr_size_new(lua_popinteger(L))
      );
    }

    if (lua_hasnumberfield(L, 3, "weight")) {
      pango_attr_list_insert(
        text.attributes,
        pango_attr_weight_new(lua_popinteger(L))
      );
    }

    if (lua_hasnumberfield(L, 3, "italics")) {
      pango_attr_list_insert(
        text.attributes,
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
        text.attributes,
        pango_attr_foreground_new(
          ((guint16) red8 << 8) | red8,
          ((guint16) green8 << 8) | green8,
          ((guint16) blue8 << 8) | blue8
        )
      );
    }

    if (lua_hasnumberfield(L, 3, "opacity")) {
      pango_attr_list_insert(
        text.attributes,
        pango_attr_foreground_alpha_new(0xffff * lua_popnumber(L))
      );
    }

    if (lua_hasstringfield(L, 3, "align")) {
      const char* alignment = lua_popstring(L);

      if (g_str_equal(alignment, "left")) {
        text.alignment = PANGO_ALIGN_LEFT;
      } else if (g_str_equal(alignment, "center")) {
        text.alignment = PANGO_ALIGN_CENTER;
      } else if (g_str_equal(alignment, "right")) {
        text.alignment = PANGO_ALIGN_RIGHT;
      } else {
        log_warning("%s", lua_field_error_msg(L, "align", "invalid value"));
      }
    }

    if (lua_hasfield(L, 3, "ellipsis")) {
      int ellipsis_type = lua_type(L, -1);

      if (ellipsis_type == LUA_TBOOLEAN) {
        text.ellipsize =
          lua_toboolean(L, -1) ? PANGO_ELLIPSIZE_END : PANGO_ELLIPSIZE_NONE;
      } else if (ellipsis_type == LUA_TSTRING) {
        const char* ellipsis = lua_tostring(L, -1);

        if (g_str_equal(ellipsis, "start")) {
          text.ellipsize = PANGO_ELLIPSIZE_START;
        } else if (g_str_equal(ellipsis, "middle")) {
          text.ellipsize = PANGO_ELLIPSIZE_MIDDLE;
        } else if (g_str_equal(ellipsis, "end")) {
          text.ellipsize = PANGO_ELLIPSIZE_END;
        } else {
          log_warning(
            "%s",
            lua_field_error_msg(L, "ellipsis", "invalid value")
          );
        }
      } else {
        log_warning("%s", lua_field_error_msg_bad_type(L, "ellipsis", -1));
      }

      lua_pop(L, 1);
    }

    if (lua_hasstringfield(L, 3, "wrap")) {
      const char* wrap = lua_popstring(L);

      if (g_str_equal(wrap, "char")) {
        text.wrap = PANGO_WRAP_CHAR;
      } else if (g_str_equal(wrap, "word")) {
        text.wrap = PANGO_WRAP_WORD;
      } else if (g_str_equal(wrap, "auto")) {
        text.wrap = PANGO_WRAP_WORD_CHAR;
      } else {
        log_warning("%s", lua_field_error_msg(L, "wrap", "invalid value"));
      }
    }
  }

  catnip_canvas_text_draw(canvas, &text);
  pango_attr_list_unref(text.attributes);

  return 0;
}
