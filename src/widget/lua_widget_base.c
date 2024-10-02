#include "lua_widget_base.h"
#include "widget/lua_widget_root.h"

void
catnip_lua_widget_base_request_layout(struct catnip_lua_widget_base* base)
{
  if (base->root != NULL) {
    catnip_lua_widget_root_request_layout(base->root);
  }
}

void
catnip_lua_widget_base_request_draw(struct catnip_lua_widget_base* base)
{
  if (base->root != NULL) {
    catnip_lua_widget_root_request_draw(base->root);
  }
}
