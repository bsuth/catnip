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

enum catnip_widget_base_unit {
  CATNIP_WIDGET_BASE_UNIT_NONE,
  CATNIP_WIDGET_BASE_UNIT_PX,
  CATNIP_WIDGET_BASE_UNIT_PERCENT,
};

struct catnip_widget_base {
  struct catnip_widget_root* root;
  struct catnip_widget_base* parent;
  void* data;
  enum catnip_widget_base_type type;
  struct catnip_lua_widget_base_mt* mt;

  struct {
    int x;
    enum catnip_widget_base_unit x_unit;
    int y;
    enum catnip_widget_base_unit y_unit;
    int width;
    enum catnip_widget_base_unit width_unit;
    int height;
    enum catnip_widget_base_unit height_unit;
  } styles;

  struct {
    int x;
    int y;
    int width;
    int height;
  } computed;
};

void
catnip_widget_base_request_layout(struct catnip_widget_base* base);

void
catnip_widget_base_request_draw(struct catnip_widget_base* base);

#endif
