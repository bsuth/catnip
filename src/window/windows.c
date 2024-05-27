#include "windows.h"
#include "seat.h"
#include "window/window.h"

struct wl_list catnip_windows;

void
catnip_windows_init()
{
  wl_list_init(&catnip_windows);
}

struct catnip_window*
catnip_windows_get_focused()
{
  struct wlr_xdg_toplevel* focused_toplevel =
    wlr_xdg_toplevel_try_from_wlr_surface(
      catnip_seat->keyboard_state.focused_surface
    );

  if (focused_toplevel != NULL) {
    struct catnip_window* window = NULL;
    wl_list_for_each(window, &catnip_windows, link)
    {
      if (window->xdg_toplevel == focused_toplevel) {
        return window;
      }
    }
  }

  return NULL;
}
