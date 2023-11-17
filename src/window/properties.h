#ifndef CATNIP_WINDOW_PROPERTIES_H
#define CATNIP_WINDOW_PROPERTIES_H

#include "window.h"

int
catnip_window_get_x(struct catnip_window* window);

int
catnip_window_get_y(struct catnip_window* window);

int
catnip_window_get_width(struct catnip_window* window);

int
catnip_window_get_height(struct catnip_window* window);

bool
catnip_window_get_focused(struct catnip_window* window);

bool
catnip_window_get_maximized(struct catnip_window* window);

bool
catnip_window_get_fullscreen(struct catnip_window* window);

void
catnip_window_set_x(struct catnip_window* window, int new_x);

void
catnip_window_set_y(struct catnip_window* window, int new_y);

void
catnip_window_set_width(struct catnip_window* window, int new_width);

void
catnip_window_set_height(struct catnip_window* window, int new_height);

void
catnip_window_set_focused(struct catnip_window* window, bool new_focused);

void
catnip_window_set_maximized(struct catnip_window* window, bool new_maximized);

void
catnip_window_set_fullscreen(struct catnip_window* window, bool new_fullscreen);

#endif
