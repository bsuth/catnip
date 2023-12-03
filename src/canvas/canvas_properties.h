#ifndef CATNIP_CANVAS_PROPERTIES_H
#define CATNIP_CANVAS_PROPERTIES_H

#include "canvas.h"

int
catnip_canvas_get_x(struct catnip_canvas* canvas);

void
catnip_canvas_set_x(struct catnip_canvas* canvas, int new_x);

int
catnip_canvas_get_y(struct catnip_canvas* canvas);

void
catnip_canvas_set_y(struct catnip_canvas* canvas, int new_y);

int
catnip_canvas_get_width(struct catnip_canvas* canvas);

void
catnip_canvas_set_width(struct catnip_canvas* canvas, int new_width);

int
catnip_canvas_get_height(struct catnip_canvas* canvas);

void
catnip_canvas_set_height(struct catnip_canvas* canvas, int new_height);

bool
catnip_canvas_get_visible(struct catnip_canvas* canvas);

void
catnip_canvas_set_visible(struct catnip_canvas* canvas, bool new_visible);

#endif
