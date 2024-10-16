#include "lua_widget_tostring.h"
#include <lauxlib.h>
#include <pango/pangocairo.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_tostring__gc(lua_State* L)
{
  struct catnip_lua_widget_tostring* tostring = lua_touserdata(L, 1);

  luaL_unref(L, LUA_REGISTRYINDEX, tostring->ref);
  free(tostring->layout);
  g_object_unref(tostring->context);

  catnip_lua_widget_base_cleanup(L, &tostring->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_tostring_mt[] = {
  {"__gc", catnip_lua_widget_tostring__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_tostring_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.tostring");
  luaL_setfuncs(L, catnip_lua_widget_tostring_mt, 0);
  lua_pop(L, 1);
}

struct catnip_lua_widget_tostring*
catnip_lua_widget_lua_tostring(lua_State* L, int idx)
{
  const char* text = lua_tostring(L, idx);
  lua_pushvalue(L, idx);

  struct catnip_lua_widget_tostring* tostring =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_tostring));
  luaL_setmetatable(L, "catnip.widget.tostring");

  catnip_lua_widget_base_setup(L, &tostring->base);
  tostring->base.type = CATNIP_LUA_WIDGET_TOSTRING;

  PangoFontMap* fontmap = pango_cairo_font_map_get_default();
  tostring->context = pango_font_map_create_context(fontmap);
  tostring->layout = pango_layout_new(tostring->context);

  lua_insert(L, -2); // move userdata below the value
  tostring->text = text;
  tostring->ref = luaL_ref(L, LUA_REGISTRYINDEX);

  if (tostring->text != NULL) {
    pango_layout_set_text(tostring->layout, tostring->text, -1);
  }

  // NOTE: Leaves `tostring` on top of the stack!
  return tostring;
}

void
catnip_lua_widget_tostring_draw(
  lua_State* L,
  struct catnip_lua_widget_tostring* tostring,
  cairo_t* cr
)
{
  // TODO: inherit properties (+ valign?)

  cairo_save(cr);
  cairo_move_to(cr, tostring->base.computed.x, tostring->base.computed.y);
  pango_cairo_show_layout(cr, tostring->layout);
  cairo_restore(cr);
}
