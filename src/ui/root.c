#include "root.h"
#include "pixman.h"
#include "server/scene.h"
#include "ui/properties.h"
#include <drm_fourcc.h>
#include <stdio.h>
#include <stdlib.h>
#include <wlr/interfaces/wlr_buffer.h>

// -----------------------------------------------------------------------------
// Root Buffer
// -----------------------------------------------------------------------------

static void
ui_root_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
  struct ui_root* root = wl_container_of(wlr_buffer, root, buffer);
  free(root);
}

static bool
ui_root_buffer_begin_data_ptr_access(
  struct wlr_buffer* wlr_buffer,
  uint32_t flags,
  void** data,
  uint32_t* format,
  size_t* stride
)
{
  struct ui_root* root = wl_container_of(wlr_buffer, root, buffer);

  *data = (void*) cairo_image_surface_get_data(root->surface);
  *format = DRM_FORMAT_ARGB8888;
  *stride = cairo_image_surface_get_stride(root->surface);

  return true;
}

static void
ui_root_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

static const struct wlr_buffer_impl ui_root_buffer_impl = {
  .destroy = ui_root_buffer_destroy,
  .begin_data_ptr_access = ui_root_buffer_begin_data_ptr_access,
  .end_data_ptr_access = ui_root_buffer_end_data_ptr_access,
};

// -----------------------------------------------------------------------------
// Root
// -----------------------------------------------------------------------------

struct ui_root*
create_ui_root()
{
  struct ui_root* root = calloc(1, sizeof(struct ui_root));

  wlr_buffer_init(&root->buffer, &ui_root_buffer_impl, 0, 0);
  root->scene_tree = wlr_scene_tree_create(&server_scene->tree);
  root->scene_buffer = wlr_scene_buffer_create(root->scene_tree, &root->buffer);

  root->width.type = UI_DIMENSION_TYPE_AUTO;
  root->height.type = UI_DIMENSION_TYPE_AUTO;
  root->visible = true;

  return root;
}

// -----------------------------------------------------------------------------
// Refresh
// -----------------------------------------------------------------------------

static void
ui_root_resize(struct ui_root* root, int new_width, int new_height)
{
  if (root->surface != NULL) {
    cairo_surface_destroy(root->surface);
  }

  if (root->cr != NULL) {
    cairo_destroy(root->cr);
  }

  root->surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, new_width, new_height);
  root->cr = cairo_create(root->surface);

  root->buffer.width = new_width;
  root->buffer.height = new_height;
}

static void
ui_root_recompute(struct ui_root* root)
{
  if (root->width.type == UI_DIMENSION_TYPE_PX) {
    root->computed_width = root->width.value;
  } else if (root->width.type == UI_DIMENSION_TYPE_PERCENT) {
    root->computed_width = root->width.value;
  } else {
    root->computed_width = 0;
  }

  if (root->height.type == UI_DIMENSION_TYPE_PX) {
    root->computed_height = root->height.value;
  } else {
    root->computed_height = 0;
  }
}

void
ui_root_refresh(struct ui_root* root)
{
  // TODO: do not do this on every refresh. its costly...
  ui_root_recompute(root);

  if (root->computed_width != root->buffer.width || root->computed_height != root->buffer.height) {
    ui_root_resize(root, root->computed_width, root->computed_height);

    // Unset buffer to force full update on refresh. Required to make sure that
    // root->scene_buffer->node.visible is updated properly.
    wlr_scene_buffer_set_buffer_with_damage(root->scene_buffer, NULL, NULL);
  }

  wlr_scene_buffer_set_buffer_with_damage(
    root->scene_buffer,
    &root->buffer,
    NULL
  );
}
