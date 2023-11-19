#include "properties.h"
#include "cursor.h"
#include "utils/time.h"
#include <wlr/types/wlr_xcursor_manager.h>

int
catnip_cursor_get_x()
{
  return catnip_cursor->x;
}

void
catnip_cursor_set_x(int new_x)
{
  wlr_cursor_warp_closest(catnip_cursor, NULL, new_x, catnip_cursor_get_y());
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
  catnip_cursor_update(get_time_msec());
}
