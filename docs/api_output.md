# `output`

A userdata representing a physical output (i.e. monitor, screen, etc.).

In catnip, outputs are essentially just viewports into the 2D window space. Any
windows or canvases that intersect the bounding box defined by the output's
`x`, `y`, `width`, and `height` determine what is displayed on the output.

```lua
!local catnip = require('catnip')
!
for output in catnip.outputs do
    print(output.id)
end
```

### [Fields](#fields) { #- }

- [`output.id`](#outputid)
- [`output.data`](#outputdata)
- [`output.x`](#outputx)
- [`output.y`](#outputy)
- [`output.width`](#outputwidth)
- [`output.height`](#outputheight)
- [`output.refresh`](#outputrefresh)
- [`output.mode`](#outputmode)
- [`output.modes`](#outputmodes)
- [`output.scale`](#outputscale)
- [`output:subscribe(event, callback)`](#outputsubscribeevent-callback)
- [`output:unsubscribe(event, callback)`](#outputunsubscribeevent-callback)
- [`output:publish(event, ...)`](#outputpublishevent-)

### [Events](#events) { #- }

- [`create`](#create)
- [`destroy`](#destroy)

## Fields

### `output.id`

```lua
!---@class CatnipOutput
!...
---@field id number (readonly)
!...
```

The output ID. This is a unique number assigned to each output that persists
across reloads.

### `output.data`

```lua
!---@class CatnipOutput
!...
---@field data table (readonly)
!...
```

A table for users to attach custom data to the output.

The table contents are never touched by catnip itself and completely defined by
the user.

This field is readonly in the sense that users are not allowed to reassign
`data` itself (for example, `output.data = 34`). This is to ensure consistency
in case external libraries, plugins, etc are expecting this field to be a table.

### `output.x`

```lua
!---@class CatnipOutput
!...
---@field x number
!...
```

The x-coordinate of the output (in pixels).

### `output.y`

```lua
!---@class CatnipOutput
!...
---@field y number
!...
```

The y-coordinate of the output (in pixels).

### `output.width`

```lua
!---@class CatnipOutput
!...
---@field width number
!...
```

The width of the output (in pixels). More specifically, this is the width of
the window space viewport that is displayed on the output.

NOTE: Changing this affects [`output.mode`](#outputmode). If the output already
advertises a mode that matches the new [`width`](#outputwidth),
[`height`](#outputheight), and [`refresh`](#outputrefresh), it is automatically
used, otherwise a custom mode is used. Some outputs may not support custom modes.

### `output.height`

```lua
!---@class CatnipOutput
!...
---@field height number
!...
```

The height of the output (in pixels). More specifically, this is the height of
the window space viewport that is displayed on the output.

NOTE: Changing this affects [`output.mode`](#outputmode). If the output already
advertises a mode that matches the new [`width`](#outputwidth),
[`height`](#outputheight), and [`refresh`](#outputrefresh), it is automatically
used, otherwise a custom mode is used. Some outputs may not support custom modes.

### `output.refresh`

```lua
!---@class CatnipOutput
!...
---@field refresh number
!...
```

The refresh rate of the output (in mHz). This value may be set to 0 in order for
a default refresh rate to be determined automatically.

NOTE: Changing this affects [`output.mode`](#outputmode). If the output already
advertises a mode that matches the new [`width`](#outputwidth),
[`height`](#outputheight), and [`refresh`](#outputrefresh), it is automatically
used, otherwise a custom mode is used. Some outputs may not support custom modes.

### `output.mode`

```lua
!---@class CatnipOutput
!...
---@field mode CatnipOutputMode
!...

---@class CatnipOutputMode
---@field width number
---@field height number
---@field refresh number
```

The current output mode, i.e. a preset of [`width`](#outputwidth),
[`height`](#outputheight), and [`refresh`](#outputrefresh), or `nil` if a
custom mode is being used..

When setting this value, one of the userdata from [`output.modes`](#outputmodes)
is expected.

### `output.modes`

An iterator to traverse through all available [modes](#outputmode) for this
output.

```lua
!local catnip = require('catnip')
!
for output in catnip.outputs do
    print("Output %d Modes:", output.id)

    for mode in output.modes do
        print("%dx%d@%d", mode.width, mode.height, mode.refresh)
    end
end
```

When modes are advertised, outputs should be configured using one of these
modes over configuring [`width`](#outputwidth), [`height`](#outputheight),
and [`refresh`](#outputrefresh) manually, as some outputs do not support
custom modes.

When no modes are advertised, the output should be configured manually via
[`width`](#outputwidth), [`height`](#outputheight), and [`refresh`](#outputrefresh).

### `output.scale`

```lua
!---@class CatnipOutput
!...
---@field scale number
!...
```

The scale factor of the output.

### `output:subscribe(event, callback)`

Similar to [`catnip.subscribe(event, callback)`](api_catnip.md#catnipsubscribeevent-callback)
but for events published on the output itself.

### `output:unsubscribe(event, callback)`

Similar to [`catnip.unsubscribe(event, callback)`](api_catnip.md#catnipunsubscribeevent-callback)
but for events published on the output itself.

### `output:publish(event, ...)`

Publishes a output event.

This publishes a local event where callbacks registered via
[`output:subscribe(event, callback)`](#outputsubscribeevent-callback) are run.

This also publishes a global event via [`catnip.publish(event, ...)`](api_catnip.md#catnippublishevent-),
where the event is prefixed w/ `output::` and the output itself is provided
as the first argument to the callback, followed by the given varargs.

```lua
!local catnip = require('catnip')
!
-- local event
my_output:subscribe('greet', function(name)
    print('hello ' .. name)
end)

-- global event
catnip.subscribe('output::greet', function(output, name)
    print(output.id .. 'says: hello ' .. name)
end)

my_output:publish('greet', 'world')
```

## Events

Output events are published both as local events and global events.

When published as a global event, events are prefixed w/ `output::` and the
output is passed as the first argument to the callback.

```lua
!local catnip = require('catnip')
!
-- local event
my_output:subscribe('destroy', function()
    print('destroy ' .. my_output.id)
end)

-- global event
catnip.subscribe('output::destroy', function(output)
    print('destroy ' .. output.name)
end)
```

### `create`

Published when a new output has been created (connected). This is also published
after (re)loading the user config for any existing outputs.

### `destroy`

Published when an output has been destroyed (disconnected).
