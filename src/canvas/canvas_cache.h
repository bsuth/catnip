#ifndef CATNIP_CANVAS_CACHE_H
#define CATNIP_CANVAS_CACHE_H

#include <stdbool.h>
#include <wayland-util.h>

struct catnip_canvas_cache {
  struct wl_list entries;
};

struct catnip_canvas_cache*
catnip_canvas_cache_create();

void
catnip_canvas_cache_destroy(struct catnip_canvas_cache* cache);

void
catnip_canvas_cache_clear(struct catnip_canvas_cache* cache);

void
catnip_canvas_cache_reset(struct catnip_canvas_cache* cache);

struct catnip_canvas_png*
catnip_canvas_cache_png(struct catnip_canvas_cache* cache, const char* path);

struct catnip_canvas_svg*
catnip_canvas_cache_svg(
  struct catnip_canvas_cache* cache,
  const char* document,
  const char* styles
);

#endif
