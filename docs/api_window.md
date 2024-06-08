# `window`

Windows are clients to the Wayland server that need to render pixels on the
screen. They are represented by rectangles in 2D (x, y) space whose contents
are determined by the client itself.

```lua
!local catnip = require('catnip')
!
for window in catnip.windows do
    print(window.id)
end
```

### [Fields](#fields) { #- }

- [`window.id`](#windowid)
- [`window.data`](#windowdata)
- [`window.x`](#windowx)
- [`window.y`](#windowy)
- [`window.z`](#windowz)
- [`window.width`](#windowwidth)
- [`window.height`](#windowheight)
- [`window.visible`](#windowvisible)
- [`window.title`](#windowtitle)
- [`window:subscribe(event, callback)`](#windowsubscribeevent-callback)
- [`window:unsubscribe(event, callback)`](#windowunsubscribeevent-callback)
- [`window:publish(event, ...)`](#windowpublishevent-)
- [`window:destroy()`](#windowdestroy)

### [Events](#events) { #- }

- [`create`](#create)
- [`destroy`](#destroy)
- [`maximize`](#maximize)
- [`fullscreen`](#fullscreen)

## Fields

### `window.id`

```lua
!---@class CatnipWindow
!...
---@field id number (readonly)
!...
```

The window ID. This is a unique number assigned to each window that persists
across reloads.

### `window.data`

```lua
!---@class CatnipWindow
!...
---@field data table (readonly)
!...
```

A table for users to attach custom data to the window.

The table contents are never touched by catnip itself and completely defined by
the user.

This field is readonly in the sense that users are not allowed to reassign
`data` itself (for example, `window.data = 34`). This is to ensure consistency
in case external libraries, plugins, etc are expecting this field to be a table.

### `window.x`

```lua
!---@class CatnipWindow
!...
---@field x number
!...
```

The x-coordinate of the window (in pixels).

### `window.y`

```lua
!---@class CatnipWindow
!...
---@field y number
!...
```

The y-coordinate of the window (in pixels).

### `window.z`

```lua
!---@class CatnipWindow
!...
---@field z number
!...
```

The z-index of the window (1-indexed).

All windows and canvas' are rendered on top of one another in a stack, and the
z-index represents the position in this stack. A higher z-index will make the
entry appear above others with a lower z-index. Conversely, a lower z-index
will make the entry appear below others with a higher z-index.

When getting this value, the position within the stack is returned, starting
from 1.

When setting this value, any arbitrary integer may be used and the z-index will
be updated to the closest valid value. For example, setting the z-index to any
of -1, 0, or 1 all move the entry to the bottom of the stack, while a value of
999 (most likely) will move the entry to the top of the stack.

```lua
-- NOTE: This example assume there are 10 windows / canvas' total.

window.z = 0 -- move to the bottom
print(window.z) -- 1

window.z = 2 -- move to the second position
print(window.z) -- 2

window.z = 999 -- move to the top
print(window.z) -- 10
```

### `window.width`

```lua
!---@class CatnipWindow
!...
---@field width number
!...
```

The width of the window (in pixels).

### `window.height`

```lua
!---@class CatnipWindow
!...
---@field height number
!...
```

The height of the window (in pixels).

### `window.visible`

```lua
!---@class CatnipWindow
!...
---@field visible boolean
!...
```

Controls whether the window should be rendered or not.

Invisible windows do not receive user input such as mouse / keyboard events.

### `window.title`

```lua
!---@class CatnipWindow
!...
---@field title string (readonly)
!...
```

The title of the window. This is set by the client itself and is often the name
of the application.

### `window:subscribe(event, callback)`

Similar to [`catnip.subscribe(event, callback)`](api_catnip.md#catnipsubscribeevent-callback)
but for events published on the window itself.

### `window:unsubscribe(event, callback)`

Similar to [`catnip.unsubscribe(event, callback)`](api_catnip.md#catnipunsubscribeevent-callback)
but for events published on the window itself.

### `window:publish(event, ...)`

Publishes a window event.

This publishes a local event where callbacks registered via
[`window:subscribe(event, callback)`](#windowsubscribeevent-callback) are run.

This also publishes a global event via [`catnip.publish(event, ...)`](api_catnip.md#catnippublishevent-),
where the event is prefixed w/ `window::` and the window itself is provided
as the first argument to the callback, followed by the given varargs.

```lua
!local catnip = require('catnip')
!
-- local event
my_window:subscribe('greet', function(name)
    print('hello ' .. name)
end)

-- global event
catnip.subscribe('window::greet', function(window, name)
    print(window.id .. 'says: hello ' .. name)
end)

my_window:publish('greet', 'world')
```

### `window:destroy()`

Requests the window to be closed. This will trigger a [`destroy`](#windowdestroy)
event just before the window is actually closed.

## Events

Window events are published both as local events and global events.

When published as a global event, events are prefixed w/ `window::` and the
window is passed as the first argument to the callback.

```lua
!local catnip = require('catnip')
!
-- local event
my_window:subscribe('destroy', function()
    print('destroy ' .. my_window.id)
end)

-- global event
catnip.subscribe('window::destroy', function(window)
    print('destroy ' .. window.id)
end)
```

### `create`

Published when a new window has been created. This is also published after
(re)loading the user config for any existing windows.
at a higher dpi.

### `destroy`

Published when a window has been destroyed (closed).

### `maximize`

Published when a window has requested to be maximized. The definition of
"maximized" and how to handle it is left to the user.

### `fullscreen`

Published when a window has requested to be fullscreen. The definition of
"fullscreen" and how to handle it is left to the user.
