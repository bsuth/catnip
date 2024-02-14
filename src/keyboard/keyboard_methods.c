#include "keyboard_methods.h"
#include "events/event_loop.h"

static void
__catnip_keyboard_reload_keymap(void* data)
{
  struct catnip_keyboard* keyboard = data;

  struct xkb_context* xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_names(
    xkb_context,
    &keyboard->xkb_rule_names,
    XKB_KEYMAP_COMPILE_NO_FLAGS
  );

  wlr_keyboard_set_keymap(keyboard->wlr_keyboard, xkb_keymap);

  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_context);

  keyboard->xkb_keymap_event_source = NULL;
}

void
catnip_keyboard_reload_keymap(struct catnip_keyboard* keyboard)
{
  if (keyboard->xkb_keymap_event_source != NULL) {
    return;
  }

  keyboard->xkb_keymap_event_source =
    catnip_event_loop_once(__catnip_keyboard_reload_keymap, keyboard);
}
