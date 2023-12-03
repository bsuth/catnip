#include "output_properties.h"
#include "output/output_layout.h"
#include <wlr/types/wlr_output.h>

static struct wlr_output_mode*
catnip_output_get_matching_mode(
  struct catnip_output* output,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_mode* mode = NULL;

  wl_list_for_each(mode, &output->wlr_output->modes, link)
  {
    if (mode->width == width && mode->height == height && mode->refresh == refresh) {
      return mode;
    }
  }

  return NULL;
}

static void
catnip_output_set_matching_mode(
  struct catnip_output* output,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_mode* matching_mode =
    catnip_output_get_matching_mode(output, width, height, refresh);

  if (matching_mode != NULL) {
    wlr_output_set_mode(output->wlr_output, matching_mode);
  } else {
    wlr_output_set_custom_mode(output->wlr_output, width, height, refresh);
  }
}

int
catnip_output_get_x(struct catnip_output* output)
{
  return wlr_output_layout_get(catnip_output_layout, output->wlr_output)->x;
}

void
catnip_output_set_x(struct catnip_output* output, int new_x)
{
  wlr_output_layout_add(
    catnip_output_layout,
    output->wlr_output,
    new_x,
    catnip_output_get_y(output)
  );
}

int
catnip_output_get_y(struct catnip_output* output)
{
  return wlr_output_layout_get(catnip_output_layout, output->wlr_output)->y;
}

void
catnip_output_set_y(struct catnip_output* output, int new_y)
{
  wlr_output_layout_add(
    catnip_output_layout,
    output->wlr_output,
    catnip_output_get_x(output),
    new_y
  );
}

int
catnip_output_get_width(struct catnip_output* output)
{
  return output->wlr_output->width;
}

void
catnip_output_set_width(struct catnip_output* output, int new_width)
{
  catnip_output_set_matching_mode(
    output,
    new_width,
    catnip_output_get_height(output),
    catnip_output_get_refresh(output)
  );
}

int
catnip_output_get_height(struct catnip_output* output)
{
  return output->wlr_output->height;
}

void
catnip_output_set_height(struct catnip_output* output, int new_height)
{
  catnip_output_set_matching_mode(
    output,
    catnip_output_get_width(output),
    new_height,
    catnip_output_get_refresh(output)
  );
}

int
catnip_output_get_refresh(struct catnip_output* output)
{
  return output->wlr_output->refresh;
}

void
catnip_output_set_refresh(struct catnip_output* output, int new_refresh)
{
  catnip_output_set_matching_mode(
    output,
    catnip_output_get_width(output),
    catnip_output_get_height(output),
    new_refresh
  );
}

struct wlr_output_mode*
catnip_output_get_mode(struct catnip_output* output)
{
  return output->wlr_output->current_mode;
}

void
catnip_output_set_mode(
  struct catnip_output* output,
  struct wlr_output_mode* new_mode
)
{
  wlr_output_set_mode(output->wlr_output, new_mode);
}

struct wl_list*
catnip_output_get_modes(struct catnip_output* output)
{
  return &output->wlr_output->modes;
}

float
catnip_output_get_scale(struct catnip_output* output)
{
  return output->wlr_output->scale;
}

void
catnip_output_set_scale(struct catnip_output* output, float new_scale)
{
  wlr_output_set_scale(output->wlr_output, new_scale);
}
