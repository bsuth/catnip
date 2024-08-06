#ifndef CATNIP_OBJECTS_KEY_EVENT_H
#define CATNIP_OBJECTS_KEY_EVENT_H

#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>

typedef struct _catnip_key_event catnip_key_event_t;

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_key_event_t*
catnip_key_event_create(
  struct wlr_keyboard* wlr_keyboard,
  struct wlr_keyboard_key_event* wlr_key_event
);

void
catnip_key_event_destroy(catnip_key_event_t* key_event);

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

uint32_t
catnip_key_event_get_modifiers(catnip_key_event_t* key_event);

xkb_keysym_t
catnip_key_event_get_xkb_keysym(catnip_key_event_t* key_event);

const char*
catnip_key_event_get_xkb_name(catnip_key_event_t* key_event);

enum wl_keyboard_key_state
catnip_key_event_get_key_state(catnip_key_event_t* key_event);

bool
catnip_key_event_get_prevent_notify(catnip_key_event_t* key_event);

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

void
catnip_key_event_set_prevent_notify(
  catnip_key_event_t* key_event,
  bool new_prevent_notify
);

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

bool
catnip_key_event_check_keybindings(catnip_key_event_t* key_event);

#endif
