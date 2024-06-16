# `canvas`

Canvases are pixel buffers that the user may use to draw arbitrary content onto
the screen. Like [windows](api_window.md), they are represented by rectangles
in the same 2D (x, y) space and also share the same [z-index](#canvasz) stack.

```lua
!local catnip = require('catnip')
!
local canvas = catnip.canvas({
    x = 0,
    y = 0,
    z = 99,
    width = 0,
    height = 0,
    visible = true,
})
```

### [Fields](#fields) { #- }

- [`canvas.x`](#canvasx)
- [`canvas.y`](#canvasy)
- [`canvas.z`](#canvasz)
- [`canvas.width`](#canvaswidth)
- [`canvas.height`](#canvasheight)
- [`canvas.visible`](#canvasvisible)
- [`canvas:path(path)`](#canvaspathpath)
- [`canvas:rectangle(rectangle)`](#canvasrectanglerectangle)
- [`canvas:text(text, options)`](#canvastexttext-options)
- [`canvas:svg(document, options)`](#canvassvgdocument-options)
- [`canvas:png(path, options)`](#canvaspngpath-options)
- [`canvas:clear()`](#canvasclear)
- [`canvas:destroy()`](#canvasdestroy)

## Fields

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

### `canvas:path(path)`

```lua
---@param path CatnipCanvasPath

---@alias CatnipCanvasPath CatnipCanvasPathFields | CatnipCanvasPathCommand[]

---@class CatnipCanvasPathFields
---The starting x-coordinate of the path (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The starting y-coordinate of the path (in pixels) relative to the top left corner of the canvas.
---@field y number?
---Whether to close the path, i.e. connect back to the starting point.
---This is not the same as simply adding a line back to the starting point, since such a line will still have line caps.
---@field close boolean?
---The color of the path fill as a hexadecimal number.
---@field fill_color number?
---The opacity of the path fill as a number between 0-1 (inclusive).
---@field fill_opacity number?
---The color of the path stroke as a hexadecimal number.
---@field stroke_color number?
---The opacity of the path stroke as a number between 0-1 (inclusive).
---@field stroke_opacity number?
---The thickness of the path stroke (in pixels).
---@field stroke_size number?
---The caps to use for the start / end of the path stroke.
---Only applies to open (not closed) paths.
---butt = no cap applied (default)
---round = circle cap applied, whose center lies at the start / end points
---square = square cap applied, whose center lies at the start / end points
---@field stroke_cap 'butt' | 'round' | 'square'?

---[2] = the relative x-coordinate of the endpoint
---[3] = the relative y-coordinate of the endpoint
---@alias CatnipCanvasPathLine { [1]: 'line', [2]: number, [3]: number }
---[2] = the relative x-coordinate of the center of the circle
---[3] = the relative y-coordinate of the center of the circle
---[4] = the rotation amount (in radians), may be either positive or negative to indicate direction
---@alias CatnipCanvasPathArc { [1]: 'arc', [2]: number, [3]: number, [4]: number }
---[2] = the relative x-coordinate of the first control point
---[3] = the relative y-coordinate of the first control point
---[4] = the relative x-coordinate of the second control point
---[5] = the relative y-coordinate of the second control point
---[6] = the relative x-coordinate of the endpoint
---[7] = the relative y-coordinate of the endpoint
---@alias CatnipCanvasPathBezier { [1]: 'bezier', [2]: number, [3]: number, [4]: number, [5]: number, [6]: number, [7]: number }
---@alias CatnipCanvasPathCommand CatnipCanvasPathLine | CatnipCanvasPathArc | CatnipCanvasPathBezier
```

Renders a path onto the canvas.

One way to think of a path is a shape that is drawable using pen and paper
without lifting the pen from surface of the paper. In catnip, paths are
constructed using lines, arcs, and bezier curves.

The "fill" of the path always considers the enclosure created as if the path had
been closed, regardless of whether it has actually been closed.

```lua
!local catnip = require('catnip')
!
!local canvas = catnip.canvas({ ... })
!
-- Draw a rectangle with x = 10, y = 20, width = 30, height = 40
canvas:path({
    x = 10,
    y = 20,
    { 'line', 30, 0 },  -- right 30px
    { 'line', 0, 40 },  -- down  40px
    { 'line', -30, 0 }, -- left  30px
    { 'line', 0, -40 }, -- up    40px
})

-- Draw the upper half of a semicircle with center x = 10, y = 20 and radius = 30
canvas:path({
    x = 10,
    y = 20,
    { 'line', -30, 0 },        -- left 30px
    { 'arc', 30, 0, math.PI }, -- semicircle
    close = true,              -- close the path
})
```

### `canvas:rectangle(rectangle)`

```lua
---@param rectangle CatnipCanvasRectangle

---@class CatnipCanvasRectangle
---The x-coordinate of the rectangle (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the rectangle (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width of the rectangle (in pixels).
---@field width number?
---The height of the rectangle (in pixels).
---@field height number?
---The default radius for all corners (in pixels).
---This is used to control how "rounded" each corner is.
---@field radius number?
---The radius of the top left corner (in pixels).
---@field radius_top_left number?
---The radius of the top right corner (in pixels).
---@field radius_top_right number?
---The radius of the bottom right corner (in pixels).
---@field radius_bottom_right number?
---The radius of the bottom left corner (in pixels).
---@field radius_bottom_left number?
---The color of the rectangle fill as a hexadecimal number.
---@field fill_color number?
---The opacity of the rectangle fill as a number between 0-1 (inclusive).
---@field fill_opacity number?
---The color of the rectangle stroke as a hexadecimal number.
---@field stroke_color number?
---The opacity of the rectangle stroke as a number between 0-1 (inclusive).
---@field stroke_opacity number?
---The thickness of the rectangle stroke (in pixels).
---@field stroke_size number?
```

Renders a rectangle onto the canvas.

Specifying any of `rectangle.radius_xxx_yyy` (ex. `rectangle.radius_top_left`)
will take priority over `rectangle.radius` for that corner.

The center of the stroke always matches the bounds of the rectangle, i.e. half
of the stroke will lie "inside" the rectangle and half will lie "outside".

### `canvas:text(text, options)`

```lua
---@param text string
---@param options CatnipCanvasTextOptions

---@class CatnipCanvasTextOptions
---The x-coordinate of the text (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the text (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width of the bounding box (in pixels).
---@field width number?
---The height of the bounding box (in pixels).
---@field height number?
---The horizontal alignment of the text, relative to the bounding box.
---@field align ('left' | 'center' | 'right')?
---The vertical alignment of the text, relative to the bounding box.
---@field valign ('left' | 'center' | 'right')?
---Controls whether and where to truncate text and apply ellipsis in the case of overflow of the bounding box.
---@field ellipsis (boolean | 'start' | 'middle' | 'end')?
---Controls whether and how to wrap text in the case of overflow on the main axis (usually the horizontal axis).
---char = allows text to wrap at any character, including in the middle of words
---word = allows text to wrap only at word boundaries
---auto = wraps text at word boundaries when possible, otherwise breaks words when necessary
---@field wrap (boolean | 'char' | 'word' | 'auto')?
---The font family.
---A comma separated list of font families may be used to specify fallbacks in case font families are missing.
---@field font string?
---The size of the font (in pixels).
---@field size number?
---The weight, i.e. "thickness" of the font as a number between 100-1000 (inclusive).
---The default weight is 400.
---@field weight number?
---Controls whether the font should be rendered in italics.
---@field italic boolean?
---The color of the text as a hexadecimal number.
---@field color number?
---The opacity of the text as a number between 0-1 (inclusive).
---@field opacity number?
```

Renders text onto the canvas.

`options.font` should be the name of a font family as a string. To list
available font families, one can use:

```bash
fc-list --format="%{family[0]}\n" | sort | uniq
```

`options.width` and `options.height` are used to determine the bounding box of
the text, which influences the following:

- If `options.align` or `options.valign` are given, this determines the bounding
  box the text should be centered in.
- If `options.wrap` is given, this determines where the text should start
  wrapping.
- If `options.ellipsis` is given, this determines when the text should be
  considered "overflowing" and thus render ellipsis.

### `canvas:png(path, options)`

```lua
---@param path string
---@param options CatnipCanvasPngOptions

---@class CatnipCanvasPngOptions
---The x-coordinate of the PNG (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the PNG (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width to render the PNG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field width number?
---The height to render the PNG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field height number?
---Controls whether the PNG should be cached for the next canvas render cycle.
---@field cache boolean?
```

Renders a PNG image onto the canvas.

`path` should be a file path to the PNG. This may either be an absolute path or
a path relative to the parent directory of the user config.

If only one of `options.width` or `options.height` is given, the other will be
scaled automatically to preserve the aspect ratio.

```lua
!local catnip = require('catnip')
!local canvas = catnip.canvas({ ... })
!
canvas:png('wallpaper.png', {
    x = 0,
    y = 0,
    width = 1920,
    height = 1080,
})
```

By default, each canvas will cache the PNG until the canvas goes through a
"render cycle" (i.e. `canvas:clear()` + drawing + `canvas:clear()`) without
drawing the PNG. In other words, the PNG will be cached as long as it was
"used last time". This gives us decent performance when a canvas needs to be
constantly rerendered with a PNG, while still being lenient enough to make
sure memory usage stays low and PNGs are not stale (in case they are updated
while the compositor is running).

Users can opt out of caching by setting `options.cache = false`:

```lua
!local catnip = require('catnip')
!local canvas = catnip.canvas({ ... })
!
canvas:png('wallpaper.png', {
    cache = false,
})
```

### `canvas:svg(document, options)`

```lua
---@param document string
---@param options CatnipCanvasSvgOptions

---@class CatnipCanvasSvgOptions
---The x-coordinate of the SVG (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the SVG (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width to render the SVG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field width number?
---The height to render the SVG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field height number?
---Controls whether the SVG should be cached for the next canvas render cycle.
---@field cache boolean?
---A [CSS stylesheet](https://gnome.pages.gitlab.gnome.org/librsvg/devel-docs/features.html#css-properties) to apply to the SVG.
---@field styles string?
```

Renders an SVG onto the canvas. Underneath the hood, catnip uses
[librsvg](https://gitlab.gnome.org/GNOME/librsvg) for SVG support. See
[here](https://gnome.pages.gitlab.gnome.org/librsvg/devel-docs/features.html)
for a full list of supported SVG and CSS features.

`document` may be either an SVG document itself, or a path to an SVG file. When
using a file path, it may either be an absolute path or a path relative to the
parent directory of the user config.

If only one of `options.width` or `options.height` is given, the other will be
scaled automatically to preserve the aspect ratio.

```lua
!local catnip = require('catnip')
!local canvas = catnip.canvas({ ... })
!
-- Rendering an SVG file
canvas:svg('wallpaper.svg', {
    x = 0,
    y = 0,
    width = 1920,
    height = 1080,
})

-- Rendering an SVG document directly
canvas:svg(
    [[
        <svg width="100" height="100" xmlns="http://www.w3.org/2000/svg">
            <circle cx="50" cy="50" r="40" stroke="green" stroke-width="4" fill="yellow" />
        </svg>
    ]],
    {
        x = 0,
        y = 0,
        width = 1920,
        height = 1080,
    }
)
```

`options.styles` may be used to apply an CSS stylesheet to the SVG. See
[here](https://gnome.pages.gitlab.gnome.org/librsvg/devel-docs/features.html#css-properties)
for a full list of supported CSS properties.

```lua
!local catnip = require('catnip')
!local canvas = catnip.canvas({ ... })
!
canvas:svg(
    [[
        <svg width="100" height="100" xmlns="http://www.w3.org/2000/svg">
            <circle cx="50" cy="50" r="40" stroke-width="4" fill="yellow" />
        </svg>
    ]],
    { styles = 'circle { stroke: green }' }
)
```

By default, each canvas will cache the SVG until the canvas goes through a
"render cycle" (i.e. `canvas:clear()` + drawing + `canvas:clear()`) without
drawing the SVG. In other words, the SVG will be cached as long as it was
"used last time". This gives us decent performance when a canvas needs to be
constantly rerendered with a SVG, while still being lenient enough to make
sure memory usage stays low and SVGs are not stale (in case they are updated
while the compositor is running).

Users can opt out of caching by setting `options.cache = false`:

NOTE: Unlike [PNGs](#canvaspngpath-options), SVGs are cached based on both the
document _and_ any provided styles (via `options.styles`). This is due to a
limitation on how [librsvg](https://gitlab.gnome.org/GNOME/librsvg) applies
styles.

```lua
!local catnip = require('catnip')
!local canvas = catnip.canvas({ ... })
!
canvas:svg('wallpaper.svg', {
    cache = false,
})
```

### `canvas:clear()`

Clears the canvas, removing anything previously drawn.

### `canvas:destroy()`

Destroys the canvas immediately. After this is called, the canvas will no longer be visible
nor useable.

This is automatically called when the canvas is garbage collected.
