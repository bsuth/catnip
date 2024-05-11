# `catnip`

Catnip exposes its API through a single entrypoint via the `catnip` module. This
module is always loaded directly via
[`package.loaded`](https://www.lua.org/manual/5.1/manual.html#pdf-package.loaded)
before running the user config.

```lua
local catnip = require('catnip')
```

- [`catnip.cursor`](#catnipcursor)
- [`catnip.outputs`](#catnipoutputs)
- [`catnip.windows`](#catnipwindows)
- [`catnip.keyboards`](#catnipkeyboards)
- [`catnip.canvas`](#catnipcanvas)
- [`catnip.png`](#catnippng)
- [`catnip.svg`](#catnipsvg)
- [`catnip.subscribe`](#catnipsubscribe)
- [`catnip.unsubscribe`](#catnipunsubscribe)
- [`catnip.publish`](#catnippublish)
- [`catnip.reload`](#catnipreload)
- [`catnip.quit`](#catnipquit)

### `catnip.cursor`

The [global cursor userdata](api_cursor.md).

```lua
!local catnip = require('catnip')
!
!-- Move the cursor to (40, 100)
catnip.cursor.x = 40
catnip.cursor.y = 100
```

### `catnip.outputs`

TODO

### `catnip.windows`

TODO

### `catnip.keyboards`

TODO

### `catnip.canvas`

TODO

### `catnip.png`

TODO

### `catnip.svg`

TODO

### `catnip.subscribe`

TODO

### `catnip.unsubscribe`

TODO

### `catnip.publish`

TODO

### `catnip.reload`

TODO

### `catnip.quit`

TODO
