#include "keybindings.h"
#include "../user_config/keybindings.h"
#include "_lua.h"
#include <glib.h>
#include <lauxlib.h>
#include <lua.h>
#include <string.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>

static int
keybindings_add(lua_State* L)
{
  luaL_checktype(L, -1, LUA_TFUNCTION);

  uint32_t modifiers = 0;
  xkb_keysym_t keysym = XKB_KEY_NoSymbol;

  int num_args = lua_gettop(L);
  for (int i = 1; i < num_args; ++i) {
    const char* s = luaL_checkstring(L, i);

    if (g_str_equal(s, "Shift")) {
      modifiers |= WLR_MODIFIER_SHIFT;
    } else if (g_str_equal(s, "Control")) {
      modifiers |= WLR_MODIFIER_CTRL;
    } else if (g_str_equal(s, "Mod1") || g_str_equal(s, "Alt")) {
      modifiers |= WLR_MODIFIER_ALT;
    } else if (g_str_equal(s, "Mod2")) {
      modifiers |= WLR_MODIFIER_MOD2;
    } else if (g_str_equal(s, "Mod3")) {
      modifiers |= WLR_MODIFIER_MOD3;
    } else if (g_str_equal(s, "Mod4") || g_str_equal(s, "Super") ||
               g_str_equal(s, "Meta")) {
      modifiers |= WLR_MODIFIER_LOGO;
    } else if (g_str_equal(s, "Mod5")) {
      modifiers |= WLR_MODIFIER_MOD5;
    } else {
      xkb_keysym_t new_keysym = xkb_keysym_from_name(s, XKB_KEYSYM_NO_FLAGS);

      if (new_keysym != XKB_KEY_NoSymbol) {
        keysym = new_keysym;
      } else if (strlen(s) == 1) {
        // Allow bindings for printable characters by providing the character
        // itself instead of the name.
        const int keycode = (int)s[0];
        if (31 < keycode && keycode < 127) {
          keysym = keycode;
        }
      }
    }
  }

  if (keysym != XKB_KEY_NoSymbol) {
    add_user_keybinding(modifiers, keysym, luaL_ref(L, LUA_REGISTRYINDEX));
  }

  return 0;
}

static const struct luaL_Reg lib_keybindings[] = { { "add", keybindings_add },
                                                   { NULL, NULL } };

void
load_lib_keybindings(lua_State* L)
{
  luaL_loadlib(L, "bwc.keybindings", lib_keybindings);
}
