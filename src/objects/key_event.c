#include "key_event.h"
#include "backend.h"
#include <stdlib.h>

struct _catnip_key_event {
  uint32_t modifiers;
  xkb_keysym_t xkb_keysym;
  char xkb_name[64];
  enum wl_keyboard_key_state key_state;
  bool prevent_notify;
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct _catnip_key_event*
catnip_key_event_create(
  struct wlr_keyboard* wlr_keyboard,
  struct wlr_keyboard_key_event* wlr_key_event
)
{
  struct _catnip_key_event* key_event =
    malloc(sizeof(struct _catnip_key_event));

  key_event->modifiers = wlr_keyboard_get_modifiers(wlr_keyboard);
  key_event->key_state = wlr_key_event->state;
  key_event->prevent_notify = false;

  key_event->xkb_keysym = xkb_state_key_get_one_sym(
    wlr_keyboard->xkb_state,
    wlr_key_event->keycode + 8 // libinput -> xkbcommon
  );

  xkb_keysym_get_name(
    key_event->xkb_keysym,
    key_event->xkb_name,
    sizeof(key_event->xkb_name)
  );

  return key_event;
}

void
catnip_key_event_destroy(struct _catnip_key_event* key_event)
{
  free(key_event);
}

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

uint32_t
catnip_key_event_get_modifiers(struct _catnip_key_event* key_event)
{
  return key_event->modifiers;
}

xkb_keysym_t
catnip_key_event_get_xkb_keysym(struct _catnip_key_event* key_event)
{
  return key_event->xkb_keysym;
}

const char*
catnip_key_event_get_xkb_name(struct _catnip_key_event* key_event)
{
  return key_event->xkb_name;
}

enum wl_keyboard_key_state
catnip_key_event_get_key_state(struct _catnip_key_event* key_event)
{
  return key_event->key_state;
}

bool
catnip_key_event_get_prevent_notify(struct _catnip_key_event* key_event)
{
  return key_event->prevent_notify;
}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

void
catnip_key_event_set_prevent_notify(
  struct _catnip_key_event* key_event,
  bool new_prevent_notify
)
{
  key_event->prevent_notify = new_prevent_notify;
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

static bool
check_vt_keybindings(struct _catnip_key_event* key_event)
{
  if (key_event->xkb_keysym < XKB_KEY_XF86Switch_VT_1) {
    return false;
  }

  if (key_event->xkb_keysym > XKB_KEY_XF86Switch_VT_12) {
    return false;
  }

  if (catnip_session != NULL) {
    wlr_session_change_vt(
      catnip_session,
      key_event->xkb_keysym - XKB_KEY_XF86Switch_VT_1 + 1
    );
  }

  return true;
}

bool
catnip_key_event_check_keybindings(struct _catnip_key_event* key_event)
{
  if (key_event->key_state != WL_KEYBOARD_KEY_STATE_PRESSED) {
    return false;
  }

  if (check_vt_keybindings(key_event)) {
    return true;
  }

  return false;
}
