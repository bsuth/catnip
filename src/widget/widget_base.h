#ifndef CATNIP_WIDGET_BASE_H
#define CATNIP_WIDGET_BASE_H

#include <lua.h>

enum catnip_widget_base_type {
  CATNIP_WIDGET_ROOT,
  CATNIP_WIDGET_BLOCK,
  CATNIP_WIDGET_IMG,
  CATNIP_WIDGET_OUTPUT,
  CATNIP_WIDGET_SVG,
  CATNIP_WIDGET_TEXT,
  CATNIP_WIDGET_WINDOW,
};

struct catnip_widget_base {
  struct catnip_widget_root* root;
  struct catnip_widget_base* parent;
  void* data;
  enum catnip_widget_base_type type;
  struct catnip_lua_widget_base_mt* mt;
};

void
catnip_widget_base_request_render(struct catnip_widget_base* base);

#endif
