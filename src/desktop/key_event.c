#include "key_event.h"
#include "compositor/backend.h"
#include <xkbcommon/xkbcommon-keysyms.h>

static bool
check_vt_keybindings(struct catnip_key_event* event)
{
  if (event->xkb_keysym < XKB_KEY_XF86Switch_VT_1) {
    return false;
  }

  if (event->xkb_keysym > XKB_KEY_XF86Switch_VT_12) {
    return false;
  }

  if (catnip_session != NULL) {
    wlr_session_change_vt(
      catnip_session,
      event->xkb_keysym - XKB_KEY_XF86Switch_VT_1 + 1
    );
  }

  return true;
}

bool
catnip_key_event_check_keybindings(struct catnip_key_event* event)
{
  if (event->state != WL_KEYBOARD_KEY_STATE_PRESSED) {
    return false;
  }

  if (check_vt_keybindings(event)) {
    return true;
  }

  return false;
}
