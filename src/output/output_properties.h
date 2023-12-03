#ifndef CATNIP_OUTPUT_PROPERTIES_H
#define CATNIP_OUTPUT_PROPERTIES_H

#include "output.h"

int
catnip_output_get_x(struct catnip_output* output);

void
catnip_output_set_x(struct catnip_output* output, int new_x);

int
catnip_output_get_y(struct catnip_output* output);

void
catnip_output_set_y(struct catnip_output* output, int new_y);

int
catnip_output_get_width(struct catnip_output* output);

void
catnip_output_set_width(struct catnip_output* output, int new_width);

int
catnip_output_get_height(struct catnip_output* output);

void
catnip_output_set_height(struct catnip_output* output, int new_height);

int
catnip_output_get_refresh(struct catnip_output* output);

void
catnip_output_set_refresh(struct catnip_output* output, int new_refresh);

struct wlr_output_mode*
catnip_output_get_mode(struct catnip_output* output);

void
catnip_output_set_mode(
  struct catnip_output* output,
  struct wlr_output_mode* new_mode
);

struct wl_list*
catnip_output_get_modes(struct catnip_output* output);

float
catnip_output_get_scale(struct catnip_output* output);

void
catnip_output_set_scale(struct catnip_output* output, float new_scale);

#endif
