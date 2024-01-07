#include "canvas_properties.h"
#include "utils/wayland.h"

int
catnip_canvas_get_x(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.x;
}

void
catnip_canvas_set_x(struct catnip_canvas* canvas, int new_x)
{
  catnip_canvas_move(canvas, new_x, catnip_canvas_get_y(canvas));
}

int
catnip_canvas_get_y(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.y;
}

void
catnip_canvas_set_y(struct catnip_canvas* canvas, int new_y)
{
  catnip_canvas_move(canvas, catnip_canvas_get_x(canvas), new_y);
}

int
catnip_canvas_get_z(struct catnip_canvas* canvas)
{
  return wlr_scene_node_get_zindex(&canvas->scene_tree->node);
}

void
catnip_canvas_set_z(struct catnip_canvas* canvas, int new_z)
{
  wlr_scene_node_set_zindex(&canvas->scene_tree->node, new_z);
}

int
catnip_canvas_get_width(struct catnip_canvas* canvas)
{
  return canvas->buffer.width;
}

void
catnip_canvas_set_width(struct catnip_canvas* canvas, int new_width)
{
  catnip_canvas_resize(canvas, new_width, catnip_canvas_get_height(canvas));
}

int
catnip_canvas_get_height(struct catnip_canvas* canvas)
{
  return canvas->buffer.height;
}

void
catnip_canvas_set_height(struct catnip_canvas* canvas, int new_height)
{
  catnip_canvas_resize(canvas, catnip_canvas_get_width(canvas), new_height);
}

bool
catnip_canvas_get_visible(struct catnip_canvas* canvas)
{
  return canvas->scene_tree->node.enabled;
}

void
catnip_canvas_set_visible(struct catnip_canvas* canvas, bool new_visible)
{
  wlr_scene_node_set_enabled(&canvas->scene_tree->node, new_visible);
}
