## Core

The core API. This is meant to be relatively small but powerful. Most plugins
should be leveraging these APIs to provide higher level functionalities,
although the user is also free to use these.

- catnip
  - [x] restart
  - [x] quit
- Signals
  - [x] add listener
  - [x] remove listener
- Keybindings
  - [x] add
  - [x] remove
  - [x] clear
- Outputs
  - [ ] Position (x, y)
  - [ ] Resolution
- Clients
  - [ ] Position (x, y, z) (z-index for stacking)
  - [ ] Geometry (width, height)
  - [ ] Visible
  - [ ] Focused
  - [ ] Fullscreen
  - [ ] Maximized
- Cursor
  - [ ] Position (x, y)
  - [ ] State (ex. left_ptr)
  - [ ] Sensitivity
  - [ ] Scale (size)
  - [ ] Theme
- [ ] Server Side decorations?
- [ ] IME
- [ ] Lock Screen

### Plugin Core

Extensions of the core API that are meant specifically to provide common
functionalities to plugins.

- [ ] UI Widgets
- [ ] libuv (https://github.com/luvit/luv)
- [ ] DBus support

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

#### Status

Provides status bar support.

#### Notifications

Registers a notification daemon and exposes the underlying models. Useful in
conjunction with the UI API.

#### NetworkManager

Provides easy API to work with NetworkManager via DBus.

#### Bluez

Provides easy API to work with bluez via DBus.

#### System Stats

Provides several models for getting / setting system statuses.

- [ ] Battery
- [ ] Brightness
- [ ] Volume
- [ ] CPU
- [ ] Memory (RAM)
