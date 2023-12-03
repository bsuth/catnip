#include "canvas_properties.h"

int
catnip_canvas_get_x(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.x;
}

int
catnip_canvas_get_y(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.y;
}

int
catnip_canvas_get_width(struct catnip_canvas* canvas)
{
  return canvas->buffer.width;
}

int
catnip_canvas_get_height(struct catnip_canvas* canvas)
{
  return canvas->buffer.height;
}

bool
catnip_canvas_get_visible(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.enabled;
}

void
catnip_canvas_set_x(struct catnip_canvas* canvas, int new_x)
{
  catnip_canvas_move(canvas, new_x, catnip_canvas_get_y(canvas));
}

void
catnip_canvas_set_y(struct catnip_canvas* canvas, int new_y)
{
  catnip_canvas_move(canvas, catnip_canvas_get_x(canvas), new_y);
}

void
catnip_canvas_set_width(struct catnip_canvas* canvas, int new_width)
{
  catnip_canvas_resize(canvas, new_width, catnip_canvas_get_height(canvas));
}

void
catnip_canvas_set_height(struct catnip_canvas* canvas, int new_height)
{
  catnip_canvas_resize(canvas, catnip_canvas_get_width(canvas), new_height);
}

void
catnip_canvas_set_visible(struct catnip_canvas* canvas, bool new_visible)
{
  wlr_scene_node_set_enabled(&canvas->scene_tree->node, new_visible);
}
