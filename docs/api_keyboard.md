# `keyboard`

A userdata representing a physical keyboard.

Under the hood, catnip uses [xkbcommon](https://xkbcommon.org/doc/current/) for
keymap support.

```lua
!local catnip = require('catnip')
!
for keyboard in catnip.keyboards do
    print(keyboard.id)
end
```

### [Fields](#fields) { #- }

- [`keyboard.id`](#keyboardid)
- [`keyboard.data`](#keyboarddata)
- [`keyboard.name`](#keyboardname)
- [`keyboard.xkb_rules`](#keyboardxkb_rules)
- [`keyboard.xkb_model`](#keyboardxkb_model)
- [`keyboard.xkb_layout`](#keyboardxkb_layout)
- [`keyboard.xkb_variant`](#keyboardxkb_variant)
- [`keyboard.xkb_options`](#keyboardxkb_options)
- [`keyboard:subscribe(event, callback)`](#keyboardsubscribeevent-callback)
- [`keyboard:unsubscribe(event, callback)`](#keyboardunsubscribeevent-callback)
- [`keyboard:publish(event, ...)`](#keyboardpublishevent-)

### [Events](#events) { #- }

- [`create`](#create)
- [`destroy`](#destroy)
- [`keypress`](#keypress)
- [`keyrelease`](#keyrelease)

## Fields

### `keyboard.id`

```lua
!---@class CatnipKeyboard
!...
---@field id number (readonly)
!...
```

The keyboard ID. This is a unique number assigned to each keyboard that persists
across reloads.

### `keyboard.data`

```lua
!---@class CatnipKeyboard
!...
---@field data table (readonly)
!...
```

A table for users to attach custom data to the keyboard.

The table contents are never touched by catnip itself and completely defined by
the user.

This field is readonly in the sense that users are not allowed to reassign
`data` itself (for example, `keyboard.data = 34`). This is to ensure consistency
in case external libraries, plugins, etc are expecting this field to be a table.

### `keyboard.name`

```lua
!---@class CatnipKeyboard
!...
---@field name string (readonly)
!...
```

The name of the keyboard.

### `keyboard.xkb_rules`

```lua
!---@class CatnipKeyboard
!...
---@field xkb_rules string
!...
```

The [xkbcommon rules](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a0968f4602001f2306febd32c34bd2280)
for the current keymap.

### `keyboard.xkb_model`

```lua
!---@class CatnipKeyboard
!...
---@field xkb_model string
!...
```

The [xkbcommon model](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a1c897b49b49c7cd495db4a424bd27265)
for the current keymap.

### `keyboard.xkb_layout`

```lua
!---@class CatnipKeyboard
!...
---@field xkb_layout string
!...
```

The [xkbcommon layout](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a243bb3aa36b4ea9ca402ed330a757746)
for the current keymap.

### `keyboard.xkb_variant`

```lua
!---@class CatnipKeyboard
!...
---@field xkb_variant string
!...
```

The [xkbcommon variant](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a758e1865c002d8f4fb59b2e3dda83b66)
for the current keymap.

### `keyboard.xkb_options`

```lua
!---@class CatnipKeyboard
!...
---@field xkb_options string
!...
```

The [xkbcommon options](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a556899eafb333c440be64ede408644df)
for the current keymap.

### `keyboard:subscribe(event, callback)`

Similar to [`catnip.subscribe(event, callback)`](api_catnip.md#catnipsubscribeevent-callback)
but for events published on the keyboard itself.

### `keyboard:unsubscribe(event, callback)`

Similar to [`catnip.unsubscribe(event, callback)`](api_catnip.md#catnipunsubscribeevent-callback)
but for events published on the keyboard itself.

### `keyboard:publish(event, ...)`

Publishes a keyboard event.

This publishes a local event where callbacks registered via
[`keyboard:subscribe(event, callback)`](#keyboardsubscribeevent-callback) are run.

This also publishes a global event via [`catnip.publish(event, ...)`](api_catnip.md#catnippublishevent-),
where the event is prefixed w/ `keyboard::` and the keyboard itself is provided
as the first argument to the callback, followed by the given varargs.

```lua
!local catnip = require('catnip')
!
-- local event
my_keyboard:subscribe('greet', function(name)
    print('hello ' .. name)
end)

-- global event
catnip.subscribe('keyboard::greet', function(keyboard, name)
    print(keyboard.id .. 'says: hello ' .. name)
end)

my_keyboard:publish('greet', 'world')
```

## Events

Keyboard events are published both as local events and global events.

When published as a global event, events are prefixed w/ `keyboard::` and the
keyboard is passed as the first argument to the callback.

```lua
!local catnip = require('catnip')
!
-- local event
my_keyboard:subscribe('destroy', function()
    print('destroy ' .. my_keyboard.id)
end)

-- global event
catnip.subscribe('keyboard::destroy', function(keyboard)
    print('destroy ' .. keyboard.id)
end)
```

### `create`

Published when a new keyboard has been created (connected). This is also
published after (re)loading the user config for any existing keyboards.

### `destroy`

Published when a keyboard has been destroyed (disconnected).

### `keypress`

```lua
---@param event CatnipKeyEvent

---@class (exact) CatnipKeyEvent
---(readonly) The [xkbcommon keysym](https://xkbcommon.org/doc/current/group__keysyms.html#ga79e604a22703391bdfe212cfc10ea007).
---@field code number
---(readonly) An [xkbcommon key name](https://xkbcommon.org/doc/current/xkbcommon-keysyms_8h.html) with the `XKB_KEY_` prefix stripped.
---@field name string
---(readonly) Whether the shift modifier is active.
---@field shift boolean
---(readonly) Whether the control modifier is active.
---@field ctrl boolean
---(readonly) Whether the mod1 modifier is active.
---@field mod1 boolean
---(readonly) Whether the mod2 modifier is active.
---@field mod2 boolean
---(readonly) Whether the mod3 modifier is active.
---@field mod3 boolean
---(readonly) Whether the mod4 modifier is active.
---@field mod4 boolean
---(readonly) Whether the mod5 modifier is active.
---@field mod5 boolean
---Controls whether to notify the currently focused window about this key event.
---@field prevent_notify boolean
```

Published whenever a key on the keyboard is pressed.

### `keyrelease`

```lua
---@param event CatnipKeyEvent

---@class (exact) CatnipKeyEvent
---(readonly) The [xkbcommon keysym](https://xkbcommon.org/doc/current/group__keysyms.html#ga79e604a22703391bdfe212cfc10ea007).
---@field code number
---(readonly) An [xkbcommon key name](https://xkbcommon.org/doc/current/xkbcommon-keysyms_8h.html) with the `XKB_KEY_` prefix stripped.
---@field name string
---(readonly) Whether the shift modifier is active.
---@field shift boolean
---(readonly) Whether the control modifier is active.
---@field ctrl boolean
---(readonly) Whether the mod1 modifier is active.
---@field mod1 boolean
---(readonly) Whether the mod2 modifier is active.
---@field mod2 boolean
---(readonly) Whether the mod3 modifier is active.
---@field mod3 boolean
---(readonly) Whether the mod4 modifier is active.
---@field mod4 boolean
---(readonly) Whether the mod5 modifier is active.
---@field mod5 boolean
---Controls whether to notify the currently focused window about this key event.
---@field prevent_notify boolean
```

Published whenever a key on the keyboard is released.
