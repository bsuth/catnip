#ifndef CATNIP_WINDOWS_H
#define CATNIP_WINDOWS_H

#include <wayland-util.h>

extern struct wl_list catnip_windows;

void
catnip_windows_init();

struct catnip_window*
catnip_windows_get_focused();

#endif
