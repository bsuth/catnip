#include "key_event.h"
#include "compositor/backend.h"
#include "compositor/display.h"
#include "config.h"
#include <wlr/types/wlr_keyboard.h>
#include <xkbcommon/xkbcommon-keysyms.h>

static bool
check_reload_keybinding(struct catnip_key_event* event)
{
  if (event->xkb_keysym != XKB_KEY_r) {
    return false;
  }

  if (!(event->modifiers & WLR_MODIFIER_CTRL)) {
    return false;
  }

  if (!(event->modifiers & WLR_MODIFIER_ALT)) {
    return false;
  }

  if (!(event->modifiers & WLR_MODIFIER_LOGO)) {
    return false;
  }

  catnip_config_request_reload = true;

  return true;
}

static bool
check_quit_keybinding(struct catnip_key_event* event)
{
  if (event->xkb_keysym != XKB_KEY_q) {
    return false;
  }

  if (!(event->modifiers & WLR_MODIFIER_CTRL)) {
    return false;
  }

  if (!(event->modifiers & WLR_MODIFIER_ALT)) {
    return false;
  }

  if (!(event->modifiers & WLR_MODIFIER_LOGO)) {
    return false;
  }

  catnip_display_run = false;

  return true;
}

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

  if (check_reload_keybinding(event)) {
    return true;
  } else if (check_quit_keybinding(event)) {
    return true;
  } else if (check_vt_keybindings(event)) {
    return true;
  }

  return false;
}
