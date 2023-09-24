## Core

The core API. This is meant to be relatively small but powerful. Most plugins
should be leveraging these APIs to provide higher level functionalities,
although the user is also free to use these.

- catnip
  - [x] restart
  - [x] quit
- Events
  - [x] add listener
  - [x] remove listener
- Keybindings
  - [x] add
  - [x] remove
  - [x] clear
  - [ ] mouse bindings
- Outputs
  - [x] Position (x, y)
  - [x] Geometry (width, height)
  - [x] Refresh rate
  - [x] Current mode
  - [x] Available modes
  - [x] Scale
  - [ ] Gamma
  - [ ] Windows
  - [ ] Wallpaper
- Windows
  - [x] Local Position (x, y)
  - [x] Global Position (x, y)
  - [x] Geometry (width, height)
  - [ ] Visible
  - [ ] Focused
  - [ ] Fullscreen
  - [ ] Maximized
  - [ ] Z-index
  - [ ] Output
  - [ ] Drag / Resize events
- Cursor
  - [x] Local Position (x, y)
  - [x] Global Position (x, y)
  - [ ] Visible
  - [ ] State (ex. left_ptr)
  - [ ] Sensitivity
  - [ ] Scale (size)
  - [ ] Theme
- [ ] Layer shell
- [ ] Server Side decorations?
- [ ] IME

## Plugins

Most useful functionality should be provided by plugins. These have no more
power and use the same APIs as user configurations. There are no lifecycle APIs,
all setup should be done at `require` time.

### Official Plugins

#### Tiling Clients

Provides tiling support.

#### Floating Clients

Provides floating window support.

Supports the following client actions:

- [ ] Move (drag)
- [ ] Resize

#### Workspaces

Provides workspace support. Less powerful but easier to use than tags.

#### Tags

Provides tags support. More powerful but more difficult to use than workspaces.
