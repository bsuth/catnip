#include "lua_canvas_text.h"
#include "canvas/canvas.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>
#include <pango/pango-layout.h>
#include <pango/pangocairo.h>

struct catnip_canvas_text {
  const char* markup;
  int x;
  int y;
  int width;
  int height;
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

  pango_layout_set_width(layout, text->width);
  pango_layout_set_height(layout, text->height);
  pango_layout_set_markup(layout, text->markup, -1);
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
    .markup = luaL_checkstring(L, 2),
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
    .alignment = PANGO_ALIGN_LEFT,
    .ellipsize = PANGO_ELLIPSIZE_NONE,
    .wrap = PANGO_WRAP_WORD_CHAR,
  };

  if (lua_type(L, 3) == LUA_TTABLE) {
    lua_hasnumberfield(L, 3, "x") && (text.x = lua_popinteger(L));
    lua_hasnumberfield(L, 3, "y") && (text.y = lua_popinteger(L));
    lua_hasnumberfield(L, 3, "width")
      && (text.width = PANGO_SCALE * lua_popinteger(L));

    if (lua_hasnumberfield(L, 3, "height")) {
      text.height = lua_popinteger(L);
      (text.height > 0) && (text.height *= PANGO_SCALE);
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
          log_warning("invalid ellipsis: %s", ellipsis);
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

  return 0;
}
