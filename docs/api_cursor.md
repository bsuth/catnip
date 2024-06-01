# `cursor`

The global cursor. This is automatically registered with all pointer devices
(ex. mouse, tablet stylus, etc.) but also controllable programatically.

Under the hood, catnip uses [XCursor](https://man.archlinux.org/man/Xcursor.3),
which allows compatability with many existing cursor themes. Note that this also
means that catnip is compatible with
[XCursor Environment Variables](https://man.archlinux.org/man/Xcursor.3.en#ENVIRONMENT_VARIABLES).

```lua
local cursor = require('catnip').cursor
```

### [Fields](#fields) { #- }

- [`cursor.x`](#cursorx)
- [`cursor.y`](#cursory)
- [`cursor.name`](#cursorname)
- [`cursor.size`](#cursorsize)
- [`cursor.theme`](#cursortheme)

## Fields

### `cursor.x`

```lua
!---@class CatnipCursor
!...
---@field x number
!...
```

The x-coordinate of the cursor (in pixels).

### `cursor.y`

```lua
!---@class CatnipCursor
!...
---@field y number
!...
```

The y-coordinate of the cursor (in pixels).

### `cursor.name`

```lua
!---@class CatnipCursor
!...
---@field name string
!...
```

The [XCursor](https://man.archlinux.org/man/Xcursor.3) name to use for the
cursor.

This value should match a filename in the `cursors/` subdirectory of the current
[cursor theme](#cursortheme).

```lua
catnip.cursor.name = "pointer"
catnip.cursor.name = "grabbing"
catnip.cursor.name = "not-allowed"
catnip.cursor.name = "default"
```

### `cursor.size`

```lua
!---@class CatnipCursor
!...
---@field size number
!...
```

The size of the cursor (in pixels).

### `cursor.theme`

```lua
!---@class CatnipCursor
!...
---@field theme string
!...
```

The [XCursor theme](https://man.archlinux.org/man/Xcursor.3#THEMES) to use for
the cursor.

This value should match the directory name of the cursor theme. Some common
paths to store cursor themes include:

- `/usr/share/icons`
- `~/.local/share/icons`
- `~/.icons`

```lua
catnip.cursor.theme = "adwaita"
catnip.cursor.theme = "bibata"
catnip.cursor.theme = "phinger"
catnip.cursor.theme = "default"
```
