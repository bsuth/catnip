#include "cursor_properties.h"
#include "cursor/cursor.h"
#include "utils/time.h"
#include <stdlib.h>
#include <string.h>
#include <wlr/types/wlr_xcursor_manager.h>

static char* catnip_cursor_name = NULL;

int
catnip_cursor_get_x()
{
  return catnip_cursor->x;
}

void
catnip_cursor_set_x(int new_x)
{
  wlr_cursor_warp_closest(catnip_cursor, NULL, new_x, catnip_cursor_get_y());
  // emit event instead of update?
  catnip_cursor_update(get_time_msec());
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
  // emit event instead of update?
  catnip_cursor_update(get_time_msec());
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
    wlr_cursor_set_xcursor(catnip_cursor, catnip_cursor_manager, new_name);
    catnip_cursor_name = strdup(new_name);
  }
}

uint32_t
catnip_cursor_get_size()
{
  return catnip_cursor_manager->size;
}

void
catnip_cursor_set_size(uint32_t new_size)
{
  struct wlr_xcursor_manager* new_catnip_cursor_manager =
    wlr_xcursor_manager_create(catnip_cursor_manager->name, new_size);

  // TODO load all scales
  wlr_xcursor_manager_load(catnip_cursor_manager, 1);

  wlr_xcursor_manager_destroy(catnip_cursor_manager);
  catnip_cursor_manager = new_catnip_cursor_manager;
}

const char*
catnip_cursor_get_theme()
{
  return catnip_cursor_manager->name;
}

void
catnip_cursor_set_theme(const char* new_theme)
{
  struct wlr_xcursor_manager* new_catnip_cursor_manager =
    wlr_xcursor_manager_create(new_theme, catnip_cursor_manager->size);

  // TODO load all scales
  wlr_xcursor_manager_load(catnip_cursor_manager, 1);

  wlr_xcursor_manager_destroy(catnip_cursor_manager);
  catnip_cursor_manager = new_catnip_cursor_manager;
}
