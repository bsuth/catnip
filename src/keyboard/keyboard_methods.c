#include "keyboard_methods.h"
#include "event_loop.h"

static void
__catnip_keyboard_reload_keymap(void* data)
{
  struct catnip_keyboard* keyboard = data;

  struct xkb_rule_names xkb_rule_names = {
    .rules = keyboard->xkb_rules,
    .model = keyboard->xkb_model,
    .layout = keyboard->xkb_layout,
    .variant = keyboard->xkb_variant,
    .options = keyboard->xkb_options,
  };

  struct xkb_context* xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_names(
    xkb_context,
    &xkb_rule_names,
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

  keyboard->xkb_keymap_event_source = wl_event_loop_add_idle(
    catnip_event_loop,
    __catnip_keyboard_reload_keymap,
    keyboard
  );
}
