#include "canvas_cache.h"
#include "canvas/canvas_png.h"
#include "canvas/canvas_svg.h"
#include "extensions/string.h"
#include <stdlib.h>

enum catnip_canvas_cache_entry_type {
  CATNIP_CANVAS_CACHE_PNG,
  CATNIP_CANVAS_CACHE_SVG,
};

struct catnip_canvas_cache_entry {
  struct wl_list link;
  enum catnip_canvas_cache_entry_type type;
  void* data;
  bool stale;
};

struct catnip_canvas_cache*
catnip_canvas_cache_create()
{
  struct catnip_canvas_cache* cache =
    calloc(1, sizeof(struct catnip_canvas_cache));

  wl_list_init(&cache->entries);

  return cache;
}

static void
catnip_canvas_cache_entry_destroy(struct catnip_canvas_cache_entry* entry)
{
  wl_list_remove(&entry->link);

  switch (entry->type) {
    case CATNIP_CANVAS_CACHE_PNG:
      catnip_canvas_png_destroy(entry->data);
      break;
    case CATNIP_CANVAS_CACHE_SVG:
      catnip_canvas_svg_destroy(entry->data);
      break;
  }

  free(entry);
}

void
catnip_canvas_cache_destroy(struct catnip_canvas_cache* cache)
{
  struct catnip_canvas_cache_entry* entry;
  struct catnip_canvas_cache_entry* tmp;

  wl_list_for_each_safe(entry, tmp, &cache->entries, link)
  {
    catnip_canvas_cache_entry_destroy(entry);
  }

  wl_list_remove(&cache->entries);
  free(cache);
}

void
catnip_canvas_cache_clear(struct catnip_canvas_cache* cache)
{
  struct catnip_canvas_cache_entry* entry;
  struct catnip_canvas_cache_entry* tmp;

  wl_list_for_each_safe(entry, tmp, &cache->entries, link)
  {
    if (entry->stale) {
      catnip_canvas_cache_entry_destroy(entry);
    }
  }
}

void
catnip_canvas_cache_reset(struct catnip_canvas_cache* cache)
{
  struct catnip_canvas_cache_entry* entry;

  wl_list_for_each(entry, &cache->entries, link)
  {
    entry->stale = true;
  }
}

struct catnip_canvas_png*
catnip_canvas_cache_png(struct catnip_canvas_cache* cache, const char* path)
{
  struct catnip_canvas_cache_entry* entry;

  wl_list_for_each(entry, &cache->entries, link)
  {
    if (entry->type == CATNIP_CANVAS_CACHE_PNG) {
      struct catnip_canvas_png* png = entry->data;

      if (streq(path, png->path)) {
        entry->stale = false;
        return png;
      }
    }
  }

  struct catnip_canvas_png* png = catnip_canvas_png_create(path);

  entry = calloc(1, sizeof(struct catnip_canvas_cache_entry));
  entry->type = CATNIP_CANVAS_CACHE_PNG;
  entry->data = png;
  entry->stale = false;
  wl_list_insert(&cache->entries, &entry->link);

  return png;
}

struct catnip_canvas_svg*
catnip_canvas_cache_svg(
  struct catnip_canvas_cache* cache,
  const char* document,
  const char* styles
)
{
  struct catnip_canvas_cache_entry* entry;

  wl_list_for_each(entry, &cache->entries, link)
  {
    if (entry->type == CATNIP_CANVAS_CACHE_SVG) {
      struct catnip_canvas_svg* svg = entry->data;

      bool has_matching_styles = styles == NULL && svg->styles == NULL
        || styles != NULL && svg->styles != NULL && streq(styles, svg->styles);

      if (has_matching_styles && streq(document, svg->document)) {
        entry->stale = false;
        return svg;
      }
    }
  }

  struct catnip_canvas_svg* svg = catnip_canvas_svg_create(document, styles);

  entry = calloc(1, sizeof(struct catnip_canvas_cache_entry));
  entry->type = CATNIP_CANVAS_CACHE_SVG;
  entry->data = svg;
  entry->stale = false;
  wl_list_insert(&cache->entries, &entry->link);

  return svg;
}
