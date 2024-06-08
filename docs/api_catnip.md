# `catnip`

Catnip exposes its API through a single entrypoint: the `catnip` module. This
module is always preloaded before running the user config
(via [`package.loaded`](https://www.lua.org/manual/5.1/manual.html#pdf-package.loaded)).

```lua
local catnip = require('catnip')
```

### [Fields](#fields) { #- }

- [`catnip.cursor`](#catnipcursor)
- [`catnip.keyboards`](#catnipkeyboards)
- [`catnip.outputs`](#catnipoutputs)
- [`catnip.windows`](#catnipwindows)
- [`catnip.focused`](#catnipfocused)
- [`catnip.canvas(options)`](#catnipcanvasoptions)
- [`catnip.subscribe(event, callback)`](#catnipsubscribeevent-callback)
- [`catnip.unsubscribe(event, callback)`](#catnipunsubscribeevent-callback)
- [`catnip.publish(event, ...)`](#catnippublishevent-)
- [`catnip.reload()`](#catnipreload)
- [`catnip.quit()`](#catnipquit)

### [Global Events](#global-events) { #- }

- [`tick`](#tick)
- [`reload`](#reload)
- [`quit`](#quit)

## Fields

### `catnip.cursor`

The global [cursor](api_cursor.md).

### `catnip.keyboards`

An iterator to traverse through all currently available [keyboards](api_keyboard.md).

```lua
!local catnip = require('catnip')
!
for keyboard in catnip.keyboards do
    print(keyboard.id)
end
```

### `catnip.outputs`

An iterator to traverse through all currently available [outputs](api_output.md).

```lua
!local catnip = require('catnip')
!
for output in catnip.outputs do
    print(output.id)
end
```

### `catnip.windows`

An iterator to traverse through all currently available [windows](api_window.md).

```lua
!local catnip = require('catnip')
!
for window in catnip.windows do
    print(window.id)
end
```

### `catnip.focused`

The currently focused window, or `nil` if no window is focused.

```lua
!local catnip = require('catnip')
!
catnip.subscribe('window::create', function(window)
    catnip.focused = window -- auto focus new windows
end)
```

```lua
!local catnip = require('catnip')
!
local function clear_focus()
    catnip.focused = nil -- unfocus the currently focused window
end
```

### `catnip.canvas(options)`

```lua
---@param options? CatnipCanvasOptions
---@return CatnipCanvas

---@class CatnipCanvasOptions
---The x-coordinate of the canvas (in pixels).
---@field x number?
---The y-coordinate of the canvas (in pixels).
---@field y number?
---The z-index of the canvas (1-indexed).
---@field z number?
---The width of the canvas (in pixels).
---@field width number?
---The height of the canvas (in pixels).
---@field height number?
---Controls whether the canvas should be rendered or not.
---@field visible boolean?
```

Creates a new [canvas](api_canvas.md).

```lua
!local catnip = require('catnip')
!
local canvas = catnip.canvas({
    width = 200,
    height = 200,
})
```

### `catnip.subscribe(event, callback)`

```lua
---@param event string
---@param callback fun(...)
---@return func(...)
```

Registers a callback to be run whenever `event` is [published](#catnippublishevent-).

Returns the callback itself, which may be used to [`unsubscribe`](#catnipunsubscribeevent-callback).

```lua
!local catnip = require('catnip')
!
local callback = catnip.subscribe('my_event', function()
    print('hello world')
end)

catnip.publish('my_event') -- "hello world"

catnip.unsubscribe('my_event', callback)

catnip.publish('my_event') -- (nothing printed)
```

Varargs passed to [`catnip.publish(event, ...)`](#catnippublishevent-) are
available as arguments in `callback`:

```lua
!local catnip = require('catnip')
!
catnip.subscribe('my_event', function(name)
    print('hello ' .. name)
end)

catnip.publish('my_event', 'mom') -- "hello mom"
```

Callbacks cannot be registered more than once. If catnip detects that `callback`
has already been subscribed to `event`, this is a noop.

### `catnip.unsubscribe(event, callback)`

```lua
---@param event string
---@param callback fun(...)
```

Unregisters a previously [subscribed](#catnipsubscribeevent-callback) callback.

```lua
!local catnip = require('catnip')
!
local callback = catnip.subscribe('my_event', function()
    print('hello world')
end)

catnip.publish('my_event') -- "hello world"

catnip.unsubscribe('my_event', callback)

catnip.publish('my_event') -- (nothing printed)
```

If the callback was never registered to `event`, this is a noop.

### `catnip.publish(event, ...)`

```lua
---@param event string
---@param ... any
```

Runs all callbacks registered via [`catnip.subscribe(event, callback)`](#catnipsubscribeevent-callback).

```lua
!local catnip = require('catnip')
!
local callback = catnip.subscribe('my_event', function()
    print('hello world')
end)

catnip.publish('my_event') -- "hello world"
```

The provided varargs are passed as arguments to each callback:

```lua
!local catnip = require('catnip')
!
catnip.subscribe('my_event', function(name)
    print('hello ' .. name)
end)

catnip.publish('my_event', 'mom') -- "hello mom"
```

### `catnip.reload()`

Queues the compositor to reload the user config after the current event loop
tick has finished executing (i.e. after the next [`tick`](#tick) event).

Reloading the user config creates a new Lua state. The current Lua state is not
closed unless the new Lua state loads successfully. This means that if an error
is thrown while reloading the user config, this is essentially a noop.

In order to prevent infinite reloading, catnip does not allow calling this
function while loading the user config. Instead, catnip will simply log
a warning and do nothing.

### `catnip.quit()`

Queues the compositor to terminate after the current event loop tick has
finished executing (i.e. after the next [`tick`](#tick) event).

## Global Events

### `tick`

Published at the end of every event loop cycle. It is particularly useful for
integrating other event loops into catnip, such as [luv](https://github.com/luvit/luv).

```lua
!local catnip = require('catnip')
!local uv = require('luv')
!
catnip.subscribe('tick', function()
    uv.run('nowait')
end)
```

### `reload`

Published just before reloading the user config.

```lua
!local catnip = require('catnip')
!
catnip.subscribe('reload', function()
    print('reloading user config')
end)
```

### `quit`

Published just before terminating the compositor.

```lua
!local catnip = require('catnip')
!
catnip.subscribe('quit', function()
    print('goodbye world')
end)
```
