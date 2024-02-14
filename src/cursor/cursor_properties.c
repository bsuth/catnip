#include "cursor_properties.h"
#include "cursor/cursor.h"
#include "cursor/cursor_methods.h"
#include "output/output.h"
#include "utils/time.h"
#include <stdlib.h>
#include <string.h>

static void
catnip_cursor_load_scaled_themes()
{
  // Always load scale 1
  wlr_xcursor_manager_load(catnip_xcursor_manager, 1);

  // Ensure we have loaded a scaled theme for each output's scale
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    wlr_xcursor_manager_load(catnip_xcursor_manager, output->wlr_output->scale);
  }
}

int
catnip_cursor_get_x()
{
  return catnip_cursor->x;
}

void
catnip_cursor_set_x(int new_x)
{
  wlr_cursor_warp_closest(catnip_cursor, NULL, new_x, catnip_cursor_get_y());
  catnip_cursor_notify(get_time_msec());
}

int
catnip_cursor_get_y()
{
  return catnip_cursor->y;
}

void
catnip_cursor_set_y(int new_y)
{
  wlr_cursor_warp_closest(catnip_cursor, NULL, catnip_cursor_get_x(), new_y);
  catnip_cursor_notify(get_time_msec());
}

const char*
catnip_cursor_get_name()
{
  return catnip_cursor_name;
}

void
catnip_cursor_set_name(const char* new_name)
{
  if (catnip_cursor_name != NULL) {
    free(catnip_cursor_name);
    catnip_cursor_name = NULL;
  }

  if (new_name == NULL) {
    wlr_cursor_unset_image(catnip_cursor);
  } else {
    catnip_cursor_name = strdup(new_name);
    wlr_cursor_set_xcursor(catnip_cursor, catnip_xcursor_manager, new_name);
  }
}

uint32_t
catnip_cursor_get_size()
{
  return catnip_xcursor_manager->size;
}

void
catnip_cursor_set_size(uint32_t new_size)
{
  struct wlr_xcursor_manager* new_wlr_xcursor_manager =
    wlr_xcursor_manager_create(catnip_cursor_get_theme(), new_size);

  catnip_cursor_load_scaled_themes();

  wlr_xcursor_manager_destroy(catnip_xcursor_manager);
  catnip_xcursor_manager = new_wlr_xcursor_manager;
}

const char*
catnip_cursor_get_theme()
{
  return catnip_xcursor_manager->name;
}

void
catnip_cursor_set_theme(const char* new_theme)
{
  struct wlr_xcursor_manager* new_wlr_xcursor_manager =
    wlr_xcursor_manager_create(new_theme, catnip_cursor_get_size());

  catnip_cursor_load_scaled_themes();

  wlr_xcursor_manager_destroy(catnip_xcursor_manager);
  catnip_xcursor_manager = new_wlr_xcursor_manager;
}
