#include "canvas_svg.h"
#include "utils/log.h"
#include <stdbool.h>
#include <string.h>

static bool
is_svg_filename(const char* document)
{
  size_t len = strlen(document);
  return len > 3 && document[len - 4] == '.' && document[len - 3] == 's'
    && document[len - 2] == 'v' && document[len - 1] == 'g';
}

struct catnip_canvas_svg*
catnip_canvas_svg_create(const char* document, const char* styles)
{
  struct catnip_canvas_svg* svg = calloc(1, sizeof(struct catnip_canvas_svg));
  GError* error = NULL;

  svg->document = strdup(document);
  svg->styles = styles != NULL ? strdup(styles) : NULL;

  svg->rsvg = is_svg_filename(svg->document)
    ? rsvg_handle_new_from_file(svg->document, &error)
    : rsvg_handle_new_from_data(
        (const guint8*) svg,
        strlen(svg->document),
        &error
      );

  if (error != NULL) {
    log_error("failed to load svg: %s", error->message);
    g_error_free(error);
    return svg;
  }

  rsvg_handle_get_intrinsic_size_in_pixels(
    svg->rsvg,
    &svg->width,
    &svg->height
  );

  if (styles != NULL) {
    rsvg_handle_set_stylesheet(
      svg->rsvg,
      (const guint8*) styles,
      strlen(styles),
      &error
    );

    if (error != NULL) {
      log_error("failed to load svg styles: %s", error->message);
      g_error_free(error);
    }
  }

  return svg;
}

void
catnip_canvas_svg_destroy(struct catnip_canvas_svg* svg)
{
  g_object_unref(svg->rsvg);
  free(svg->document);

  if (svg->styles != NULL) {
    free(svg->styles);
  }

  free(svg);
}
