#include "keyboard.h"
#include "../display.h"
#include "../seat.h"
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>

struct wl_list bwc_keyboards;

static void
keyboard_modifiers_notify(struct wl_listener* listener, void* data)
{
  struct bwc_keyboard* keyboard =
    wl_container_of(listener, keyboard, modifiers_listener);

  /*
   * A seat can only have one keyboard, but this is a limitation of the
   * Wayland protocol - not wlroots. We assign all connected keyboards to the
   * same seat. You can swap out the underlying wlr_keyboard like this and
   * wlr_seat handles this transparently.
   */
  wlr_seat_set_keyboard(bwc_seat, keyboard->wlr_keyboard);

  wlr_seat_keyboard_notify_modifiers(bwc_seat,
                                     &keyboard->wlr_keyboard->modifiers);
}

// TODO handle real bindings
static bool
handle_keybinding(xkb_keysym_t sym)
{
  /*
   * Here we handle compositor keybindings. This is when the compositor is
   * processing keys, rather than passing them on to the client for its own
   * processing.
   *
   * This function assumes Alt is held down.
   */
  switch (sym) {
    case XKB_KEY_Escape:
      wl_display_terminate(bwc_display);
      break;
    default:
      return false;
  }
  return true;
}

static void
keyboard_key_notify(struct wl_listener* listener, void* data)
{
  struct bwc_keyboard* keyboard =
    wl_container_of(listener, keyboard, key_listener);

  struct wlr_keyboard_key_event* event = data;

  /* Translate libinput keycode -> xkbcommon */
  uint32_t keycode = event->keycode + 8;

  /* Get a list of keysyms based on the keymap for this keyboard */
  const xkb_keysym_t* syms;

  int nsyms =
    xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);

  bool handled = false;
  uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
  if ((modifiers & WLR_MODIFIER_ALT) &&
      event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    /* If alt is held down and this button was _pressed_, we attempt to
     * process it as a compositor keybinding. */
    for (int i = 0; i < nsyms; i++) {
      handled = handle_keybinding(syms[i]);
    }
  }

  if (!handled) {
    /* Otherwise, we pass it along to the client. */
    wlr_seat_set_keyboard(bwc_seat, keyboard->wlr_keyboard);
    wlr_seat_keyboard_notify_key(
      bwc_seat, event->time_msec, event->keycode, event->state);
  }
}

static void
keyboard_destroy_notify(struct wl_listener* listener, void* data)
{
  struct bwc_keyboard* keyboard =
    wl_container_of(listener, keyboard, destroy_listener);

  wl_list_remove(&keyboard->modifiers_listener.link);
  wl_list_remove(&keyboard->key_listener.link);
  wl_list_remove(&keyboard->destroy_listener.link);
  wl_list_remove(&keyboard->link);

  free(keyboard);
}

void
register_new_keyboard(struct wlr_input_device* device)
{
  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);

  struct bwc_keyboard* keyboard = calloc(1, sizeof(struct bwc_keyboard));
  keyboard->wlr_keyboard = wlr_keyboard;

  /* We need to prepare an XKB keymap and assign it to the keyboard. This
   * assumes the defaults (e.g. layout = "us"). */
  struct xkb_context* context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* keymap =
    xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);

  wlr_keyboard_set_keymap(wlr_keyboard, keymap);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  keyboard->modifiers_listener.notify = keyboard_modifiers_notify;
  wl_signal_add(&wlr_keyboard->events.modifiers, &keyboard->modifiers_listener);
  keyboard->key_listener.notify = keyboard_key_notify;
  wl_signal_add(&wlr_keyboard->events.key, &keyboard->key_listener);
  keyboard->destroy_listener.notify = keyboard_destroy_notify;
  wl_signal_add(&device->events.destroy, &keyboard->destroy_listener);

  wlr_seat_set_keyboard(bwc_seat, keyboard->wlr_keyboard);

  wl_list_insert(&bwc_keyboards, &keyboard->link);
}

void
bwc_keyboard_init()
{
  wl_list_init(&bwc_keyboards);
}
