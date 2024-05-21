# `canvas`

TODO

`catnip.canvas()`

- [`canvas.x`](#canvasx)
- [`canvas.y`](#canvasy)
- [`canvas.z`](#canvasz)
- [`canvas.width`](#canvaswidth)
- [`canvas.height`](#canvasheight)
- [`canvas.visible`](#canvasvisible)
- [`canvas.path(path)`](#canvaspathpath)
- [`canvas.rectangle(rectangle)`](#canvasrectanglerectangle)
- [`canvas.text(text, options)`](#canvastexttextoptions)
- [`canvas.svg(svg, options)`](#canvassvgsvgoptions)
- [`canvas.png(png, options)`](#canvaspngpngoptions)
- [`canvas.clear()`](#canvasclear)
- [`canvas.destroy()`](#canvasdestroy)

### `canvas.x`

```lua
!---@class CatnipCanvas
!...
---@field x number
!...
```

The x-coordinate of the canvas (in pixels).

### `canvas.y`

```lua
!---@class CatnipCanvas
!...
---@field y number
!...
```

The y-coordinate of the canvas (in pixels).

### `canvas.z`

```lua
!---@class CatnipCanvas
!...
---@field z number
!...
```

The z-index of the canvas (1-indexed).

All windows and canvas' are rendered on top of one another in a stack, and the
z-index represents the position in this stack. A higher z-index will make the
entry appear above others with a lower z-index. Conversely, a lower z-index
will make the entry appear below others with a higher z-index.

When getting this value, the position within the stack is returned, starting
from 1.

When setting this value, any arbitrary integer may be used and the z-index will
be updated to the closest valid value. For example, setting the z-index to any
of -1, 0, or 1 will move the entry to the bottom of the stack, while a value of
999 (most likely) will move the entry to the top of the stack.

```lua
-- NOTE: This example assume there are 10 windows / canvas' total.

canvas.z = 0 -- move to the bottom
print(canvas.z) -- 1

canvas.z = 2 -- move to the second position
print(canvas.z) -- 2

canvas.z = 999 -- move to the top
print(canvas.z) -- 10
```

### `canvas.width`

```lua
!---@class CatnipCanvas
!...
---@field width number
!...
```

The width of the canvas (in pixels).

### `canvas.height`

```lua
!---@class CatnipCanvas
!...
---@field height number
!...
```

The height of the canvas (in pixels).

### `canvas.visible`

```lua
!---@class CatnipCanvas
!...
---@field visible boolean
!...
```

Controls whether the canvas should be rendered or not.

### `canvas.path(path)`

TODO

### `canvas.rectangle(rectangle)`

TODO

### `canvas.text(text, options)`

TODO

### `canvas.svg(svg, options)`

TODO

### `canvas.png(png, options)`

TODO

### `canvas.clear()`

TODO

### `canvas.destroy()`

TODO
