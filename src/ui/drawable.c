#include "drawable.h"
#include "server/scene.h"
#include "ui/wlr_cairo_buffer.h"
#include <stdlib.h>

struct drawable*
create_drawable()
{
  struct drawable* drawable = malloc(sizeof(struct drawable));

  drawable->scene_tree = wlr_scene_tree_create(&server_scene->tree);
  drawable->cairo_surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 40 * 10, 40);
  drawable->cr = cairo_create(drawable->cairo_surface);
  drawable->buffer = create_wlr_cairo_buffer(drawable->cairo_surface);

  wlr_scene_buffer_create(drawable->scene_tree, &drawable->buffer->base);

  return drawable;
}
