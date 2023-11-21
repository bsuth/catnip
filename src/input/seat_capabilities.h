#ifndef CATNIP_SEAT_CAPABILITIES_H
#define CATNIP_SEAT_CAPABILITIES_H

#include <wlr/types/wlr_seat.h>

void
catnip_seat_capability_register_pointer(struct wlr_input_device* device);

void
catnip_seat_capability_register_keyboard(struct wlr_input_device* device);

void
catnip_seat_capability_register_touch(struct wlr_input_device* device);

#endif
