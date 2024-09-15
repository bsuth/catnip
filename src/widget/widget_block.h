#ifndef CATNIP_WIDGET_BLOCK_H
#define CATNIP_WIDGET_BLOCK_H

#include <cairo.h>

enum catnip_widget_block_unit {
  CATNIP_WIDGET_BLOCK_UNIT_NONE,
  CATNIP_WIDGET_BLOCK_UNIT_PX,
  CATNIP_WIDGET_BLOCK_UNIT_PERCENT,
};

struct catnip_widget_block {
  // TODO: children

  struct {
    int x;
    enum catnip_widget_block_unit x_unit;
    int y;
    enum catnip_widget_block_unit y_unit;

    int width;
    enum catnip_widget_block_unit width_unit;
    int height;
    enum catnip_widget_block_unit height_unit;

    int padding;
    int padding_top;
    int padding_left;
    int padding_bottom;
    int padding_right;

    // TODO: layout properties

    int radius;
    int radius_top_left;
    int radius_top_right;
    int radius_bottom_right;
    int radius_bottom_left;

    int bg_color;
    double bg_opacity;

    int border_color;
    double border_opacity;
    int border_width;

    // TODO: inheritable properties
    // double opacity;
    // - text properties
  } styles;

  struct {
    int x;
    int y;
    int width;
    int height;

    int content_x;
    int content_y;
    int content_width;
    int content_height;
  } computed;
};

struct catnip_widget_block*
catnip_widget_block_create();

void
catnip_widget_block_destroy(struct catnip_widget_block* block);

void
catnip_widget_block_layout(struct catnip_widget_block* block);

void
catnip_widget_block_draw(struct catnip_widget_block* block, cairo_t* cr);

#endif
