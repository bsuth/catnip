#ifndef CATNIP_CURSOR_H
#define CATNIP_CURSOR_H

#include <stdint.h>
#include <wlr/types/wlr_data_device.h>

struct catnip_cursor;

extern struct catnip_cursor* catnip_cursor;

void
catnip_cursor_init();

int
catnip_cursor_get_x();

void
catnip_cursor_set_x(int new_x);

int
catnip_cursor_get_y();

void
catnip_cursor_set_y(int new_y);

const char*
catnip_cursor_get_name();

void
catnip_cursor_set_name(const char* new_name);

uint32_t
catnip_cursor_get_size();

void
catnip_cursor_set_size(uint32_t new_size);

const char*
catnip_cursor_get_theme();

void
catnip_cursor_set_theme(const char* new_theme);

void
catnip_cursor_attach_pointer(struct wlr_input_device* device);

#endif
