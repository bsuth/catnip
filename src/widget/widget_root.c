#include "widget_root.h"
#include "compositor/event_loop.h"
#include "compositor/scene.h"
#include "widget/widget_block.h"
#include <drm_fourcc.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <wlr/interfaces/wlr_buffer.h>

// -----------------------------------------------------------------------------
// Buffer
// -----------------------------------------------------------------------------

static void
catnip_widget_root_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
}

static bool
catnip_widget_root_buffer_begin_data_ptr_access(
  struct wlr_buffer* wlr_buffer,
  uint32_t flags,
  void** data,
  uint32_t* format,
  size_t* stride
)
{
  struct catnip_widget_root* widget =
    wl_container_of(wlr_buffer, widget, wlr.buffer);

  *data = (void*) cairo_image_surface_get_data(widget->cairo.surface);
  *format = DRM_FORMAT_ARGB8888;
  *stride = cairo_image_surface_get_stride(widget->cairo.surface);

  return true;
}

static void
catnip_widget_root_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

const struct wlr_buffer_impl catnip_widget_root_buffer = {
  .destroy = catnip_widget_root_buffer_destroy,
  .begin_data_ptr_access = catnip_widget_root_buffer_begin_data_ptr_access,
  .end_data_ptr_access = catnip_widget_root_buffer_end_data_ptr_access,
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct catnip_widget_root*
catnip_widget_root_create(int initial_width, int initial_height)
{
  struct catnip_widget_root* root = malloc(sizeof(struct catnip_widget_root));

  root->widget = NULL;
  root->lua_widget = LUA_NOREF;

  root->cairo.surface = cairo_image_surface_create(
    CAIRO_FORMAT_ARGB32,
    initial_width,
    initial_height
  );

  root->cairo.cr = cairo_create(root->cairo.surface);

  wlr_buffer_init(
    &root->wlr.buffer,
    &catnip_widget_root_buffer,
    initial_width,
    initial_height
  );

  root->wlr.scene_buffer =
    wlr_scene_buffer_create(&catnip_scene->tree, &root->wlr.buffer);

  root->request.event_source = NULL;
  root->request.width = initial_width;
  root->request.height = initial_height;
  root->request.render = false;

  return root;
}

void
catnip_widget_root_destroy(struct catnip_widget_root* root)
{
  cairo_surface_destroy(root->cairo.surface);
  cairo_destroy(root->cairo.cr);

  wlr_buffer_drop(&root->wlr.buffer);
  wlr_scene_node_destroy(&root->wlr.scene_buffer->node);

  if (root->request.event_source != NULL) {
    wl_event_source_remove(root->request.event_source);
  }

  free(root);
}

// -----------------------------------------------------------------------------
// Refresh
// -----------------------------------------------------------------------------

static void
catnip_widget_root_resize(
  struct catnip_widget_root* widget,
  int new_width,
  int new_height
)
{
  cairo_surface_destroy(widget->cairo.surface);
  cairo_destroy(widget->cairo.cr);

  // Cairo does not allow an image surface to be resized after creation. Thus,
  // we have to destroy any previous surfaces / contexts and create new ones.
  widget->cairo.surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, new_width, new_height);
  widget->cairo.cr = cairo_create(widget->cairo.surface);

  widget->wlr.buffer.width = new_width;
  widget->wlr.buffer.height = new_height;

  // Unset buffer to force full update on refresh. Required to make sure that
  // widget->scene_buffer->node.visible is updated properly.
  wlr_scene_buffer_set_buffer_with_damage(widget->wlr.scene_buffer, NULL, NULL);
}

static void
catnip_widget_root_render(struct catnip_widget_root* root)
{
  cairo_save(root->cairo.cr);
  cairo_set_operator(root->cairo.cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(root->cairo.cr);
  cairo_restore(root->cairo.cr);

  if (root->widget != NULL) {
    // TODO: layout

    switch (root->widget->type) {
      case CATNIP_WIDGET_BLOCK:
        catnip_widget_block_draw(root->widget->data, root->cairo.cr);
        break;
      case CATNIP_WIDGET_IMG:
        // TODO
        break;
      case CATNIP_WIDGET_OUTPUT:
        // TODO
        break;
      case CATNIP_WIDGET_SVG:
        // TODO
        break;
      case CATNIP_WIDGET_TEXT:
        // TODO
        break;
      case CATNIP_WIDGET_WINDOW:
        // TODO
        break;
      default:
        break;
    }
  }

  wlr_scene_buffer_set_buffer_with_damage(
    root->wlr.scene_buffer,
    &root->wlr.buffer,
    NULL
  );
}

static void
catnip_widget_root_refresh(void* data)
{
  struct catnip_widget_root* root = data;

  bool needs_resize = root->request.width != root->wlr.buffer.width
    || root->request.height != root->wlr.buffer.height;

  if (needs_resize) {
    catnip_widget_root_resize(root, root->request.width, root->request.height);
  }

  if (needs_resize || root->request.render) {
    catnip_widget_root_render(root);
    root->request.render = false;
  }
}

static void
catnip_widget_root_queue_refresh(struct catnip_widget_root* root)
{
  if (root->request.event_source != NULL) {
    return; // already queued
  }

  root->request.event_source =
    wl_event_loop_add_idle(catnip_event_loop, catnip_widget_root_refresh, root);
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_widget_root_request_resize(
  struct catnip_widget_root* root,
  int new_width,
  int new_height
)
{
  root->request.width = new_width;
  root->request.height = new_height;
  catnip_widget_root_queue_refresh(root);
}

void
catnip_widget_root_request_render(struct catnip_widget_root* root)
{
  root->request.render = true;
  catnip_widget_root_queue_refresh(root);
}
