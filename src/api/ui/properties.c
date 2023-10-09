#include "properties.h"
#include "ui/properties.h"
#include "utils/log.h"
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static int
parse_percent_str(const char* str)
{
  const int len = strlen(str);

  if (str[len - 1] != '%') {
    return -1;
  }

  int percent = 0;

  for (int i = 0; i < len - 1; ++i) {
    if (str[i] < '0' || str[i] > '9') {
      return -1;
    } else {
      percent = 10 * percent + (str[i] - '0');
    }
  }

  return percent;
}

// -----------------------------------------------------------------------------
// Dimension
// -----------------------------------------------------------------------------

void
lua_get_ui_dimension(lua_State* L, struct ui_dimension* ui_dimension)
{
  if (ui_dimension->type == UI_DIMENSION_TYPE_AUTO) {
    lua_pushnil(L);
  } else if (ui_dimension->type == UI_DIMENSION_TYPE_PX) {
    lua_pushnumber(L, ui_dimension->value);
  } else if (ui_dimension->type == UI_DIMENSION_TYPE_PERCENT) {
    GString* str = g_string_new(NULL);
    g_string_printf(str, "%d%%", ui_dimension->value);
    lua_pushstring(L, str->str);
    g_string_free(str, true);
  } else if (ui_dimension->type == UI_DIMENSION_TYPE_FIT) {
    lua_pushstring(L, "fit");
  }
}

void
lua_set_ui_dimension(
  lua_State* L,
  struct ui_dimension* ui_dimension,
  const char* label
)
{
  int type = lua_type(L, -1);

  if (type == LUA_TNIL) {
    ui_dimension->type = UI_DIMENSION_TYPE_AUTO;
    return;
  }

  if (type == LUA_TNUMBER) {
    ui_dimension->type = UI_DIMENSION_TYPE_PX;
    ui_dimension->value = lua_tonumber(L, -1);
    return;
  }

  if (type != LUA_TSTRING) {
    log_warning("invalid %s type: %s", label, lua_typename(L, type));
    return;
  }

  const char* dimension_str = lua_tostring(L, -1);

  if (g_str_equal(dimension_str, "fit")) {
    ui_dimension->type = UI_DIMENSION_TYPE_FIT;
    return;
  }

  const int parsed_percent = parse_percent_str(dimension_str);
  if (parsed_percent != -1) {
    ui_dimension->type = UI_DIMENSION_TYPE_PERCENT;
    ui_dimension->value = parsed_percent;
    return;
  }

  log_warning("invalid %s string: %s", label, dimension_str);
}
