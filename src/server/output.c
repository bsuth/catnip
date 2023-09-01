#include "output.h"
#include "allocator.h"
#include "backend.h"
#include "renderer.h"
#include "scene.h"
#include <stdlib.h>

struct wlr_output_layout* bwc_output_layout;
struct wl_list bwc_outputs;

struct wl_listener backend_new_output_listener;

static void
output_frame_notify(struct wl_listener* listener, void* data)
{
  struct bwc_output* output = wl_container_of(listener, output, frame);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(bwc_scene, output->wlr_output);

  wlr_scene_output_commit(scene_output);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
output_destroy_notify(struct wl_listener* listener, void* data)
{
  struct bwc_output* output = wl_container_of(listener, output, destroy);

  wl_list_remove(&output->frame.link);
  wl_list_remove(&output->destroy.link);
  wl_list_remove(&output->link);

  free(output);
}

static void
backend_new_output_notify(struct wl_listener* listener, void* data)
{
  struct wlr_output* wlr_output = data;

  wlr_output_init_render(wlr_output, bwc_allocator, bwc_renderer);

  /* Some backends don't have modes. DRM+KMS does, and we need to set a mode
   * before we can use the output. The mode is a tuple of (width, height,
   * refresh rate), and each monitor supports only a specific set of modes. We
   * just pick the monitor's preferred mode, a more sophisticated compositor
   * would let the user configure it. */
  if (!wl_list_empty(&wlr_output->modes)) {
    struct wlr_output_mode* mode = wlr_output_preferred_mode(wlr_output);

    wlr_output_set_mode(wlr_output, mode);
    wlr_output_enable(wlr_output, true);

    if (!wlr_output_commit(wlr_output)) {
      return;
    }
  }

  struct bwc_output* output = calloc(1, sizeof(struct bwc_output));
  output->wlr_output = wlr_output;

  output->frame.notify = output_frame_notify;
  wl_signal_add(&wlr_output->events.frame, &output->frame);

  output->destroy.notify = output_destroy_notify;
  wl_signal_add(&wlr_output->events.destroy, &output->destroy);

  wl_list_insert(&bwc_outputs, &output->link);

  // Adds this to the output layout. The add_auto function arranges outputs
  // from left-to-right in the order they appear. A more sophisticated
  // compositor would let the user configure the arrangement of outputs in the
  // layout.
  //
  // The output layout utility automatically adds a wl_output global to the
  // display, which Wayland clients can see to find out information about the
  // output (such as DPI, scale factor, manufacturer, etc).
  //
  // TODO allow layout configuration
  wlr_output_layout_add_auto(bwc_output_layout, wlr_output);
}

void
bwc_output_init()
{
  bwc_output_layout = wlr_output_layout_create();
  wl_list_init(&bwc_outputs);
  backend_new_output_listener.notify = backend_new_output_notify;
  wl_signal_add(&bwc_backend->events.new_output, &backend_new_output_listener);
}
