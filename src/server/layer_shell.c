#include "layer_shell.h"
#include "server/display.h"

struct wlr_layer_shell_v1* server_layer_shell;

// static void
// create_layer_surface(struct wl_listener* listener, void* data)
// {
//   struct wlr_layer_surface_v1* wlr_layer_surface = data;
//   LayerSurface* layersurface;
//   struct wlr_layer_surface_v1_state old_state;

//   if (!wlr_layer_surface->output) {
//     wlr_layer_surface->output = selmon ? selmon->wlr_output : NULL;
//   }

//   if (!wlr_layer_surface->output) {
//     wlr_layer_surface_v1_destroy(wlr_layer_surface);
//     return;
//   }

//   layersurface = ecalloc(1, sizeof(LayerSurface));
//   layersurface->type = LayerShell;
//   LISTEN(
//     &wlr_layer_surface->surface->events.commit,
//     &layersurface->surface_commit,
//     commitlayersurfacenotify
//   );
//   LISTEN(
//     &wlr_layer_surface->events.destroy,
//     &layersurface->destroy,
//     destroylayersurfacenotify
//   );
//   LISTEN(
//     &wlr_layer_surface->events.map,
//     &layersurface->map,
//     maplayersurfacenotify
//   );
//   LISTEN(
//     &wlr_layer_surface->events.unmap,
//     &layersurface->unmap,
//     unmaplayersurfacenotify
//   );

//   layersurface->layer_surface = wlr_layer_surface;
//   layersurface->mon = wlr_layer_surface->output->data;
//   wlr_layer_surface->data = layersurface;

//   layersurface->scene_layer = wlr_scene_layer_surface_v1_create(
//     layers[wlr_layer_surface->pending.layer],
//     wlr_layer_surface
//   );
//   layersurface->scene = layersurface->scene_layer->tree;
//   layersurface->popups = wlr_layer_surface->surface->data =
//     wlr_scene_tree_create(layers[wlr_layer_surface->pending.layer]);

//   layersurface->scene->node.data = layersurface;

//   wl_list_insert(
//     &layersurface->mon->layers[wlr_layer_surface->pending.layer],
//     &layersurface->link
//   );
// }

void
init_server_layer_shell()
{
  server_layer_shell = wlr_layer_shell_v1_create(server_display);
  // wl_signal_add(&layer_shell->events.new_surface, &new_layer_shell_surface);
}
