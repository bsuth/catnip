# `window`

TODO

`catnip.windows`

- [`window.x`](#windowx)
- [`window.y`](#windowy)
- [`window.z`](#windowz)
- [`window.width`](#windowwidth)
- [`window.height`](#windowheight)
- [`window.visible`](#windowvisible)
- [`window.title`](#windowtitle)
- [`window.destroy()`](#windowdestroy)

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

TODO

### `window.destroy()`

TODO
