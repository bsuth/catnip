#ifndef CATNIP_API_UI_PROPERTIES_H
#define CATNIP_API_UI_PROPERTIES_H

#include "ui/properties.h"
#include <lua.h>

void
lua_get_ui_dimension(lua_State* L, struct ui_dimension* ui_dimension);

void
lua_set_ui_dimension(
  lua_State* L,
  struct ui_dimension* ui_dimension,
  const char* label
);

#endif
