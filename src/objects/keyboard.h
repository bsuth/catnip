#ifndef CATNIP_OBJECTS_KEYBOARD_H
#define CATNIP_OBJECTS_KEYBOARD_H

#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>

typedef struct _catnip_keyboard catnip_keyboard_t;

enum catnip_keyboard_event {
  CATNIP_KEYBOARD_EVENT_DESTROY, // catnip_keyboard_t
  CATNIP_KEYBOARD_EVENT_KEY, // catnip_key_event_t
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_keyboard_t*
catnip_keyboard_create(struct wlr_keyboard* wlr_keyboard);

void
catnip_keyboard_destroy(catnip_keyboard_t* keyboard);

void
catnip_keyboard_link(struct _catnip_keyboard* keyboard, struct wl_list* list);

void
catnip_keyboard_subscribe(
  catnip_keyboard_t* keyboard,
  enum catnip_keyboard_event event,
  struct wl_listener* listener,
  wl_notify_func_t callback
);

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

const int
catnip_keyboard_get_id(catnip_keyboard_t* keyboard);

const char*
catnip_keyboard_get_name(catnip_keyboard_t* keyboard);

const char*
catnip_keyboard_get_xkb_rules(catnip_keyboard_t* keyboard);

const char*
catnip_keyboard_get_xkb_model(catnip_keyboard_t* keyboard);

const char*
catnip_keyboard_get_xkb_layout(catnip_keyboard_t* keyboard);

const char*
catnip_keyboard_get_xkb_variant(catnip_keyboard_t* keyboard);

const char*
catnip_keyboard_get_xkb_options(catnip_keyboard_t* keyboard);

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

void
catnip_keyboard_set_xkb_rules(
  catnip_keyboard_t* keyboard,
  const char* new_xkb_rules
);

void
catnip_keyboard_set_xkb_model(
  catnip_keyboard_t* keyboard,
  const char* new_xkb_model
);

void
catnip_keyboard_set_xkb_layout(
  catnip_keyboard_t* keyboard,
  const char* new_xkb_layout
);

void
catnip_keyboard_set_xkb_variant(
  catnip_keyboard_t* keyboard,
  const char* new_xkb_variant
);

void
catnip_keyboard_set_xkb_options(
  catnip_keyboard_t* keyboard,
  const char* new_xkb_options
);

#endif
