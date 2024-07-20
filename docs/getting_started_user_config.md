# User Config

Catnip offers customization via a user config, which is simply a
[Lua](https://www.lua.org/) script with access to the
[catnip Lua API](api_catnip.md). The user config is run whenever a new Lua
state is created, which is done both at startup, as well as every time the
compositor is [reloaded](api_catnip.md#catnipreload).

When loading the user config, catnip will search the following paths in order:

1. The CLI `--config` path (ex. `catnip --config ~/mydotfiles/catnip`)
1. `$XDG_CONFIG_HOME/catnip/init.lua`
1. `~/.config/catnip/init.lua`

## Your First User Config

To create a user config, create a file in any of the search paths listed above.
For most users, the following will suffice:

```bash
touch ~/.config/catnip/init.lua
```

After doing this, ***do not run catnip***. At this point, the user config exists
and catnip will happily load it. However, since nothing is defined, we won't
be able to do anything and essentially be "locked" in the compositor. If this
happens, you may need to switch to another TTY and manually kill catnip.

We need to populate the user config with _something_. A good starting point is
to copy the [keybind example](examples_keybind.md) and at least provide a way
to quit or reload catnip:

```lua
local keybind = require('keybind') -- copy pasted from examples

keybind.release({ 'mod4', 'ctrl' }, 'r', catnip.reload)
keybind.release({ 'mod4', 'ctrl' }, 'q', catnip.quit)
```

Now we have something (barely) runnable! As you may have noticed by now,
everything in catnip must be defined by the user. This makes the compositor
extremely flexible and powerful, but it also means that...

## Catnip Sucks (for new users)

Catnip is _not_ a batteries included compositor and requires a nontrivial
amount of work before becoming usable as a daily driver. This comes as a
consequence of catnip shipping with essentially no default behavior and
requiring the user to specify all desired behavior via the relatively low level
Lua API.

This large barrier to entry is by far catnip's weakest and most unattractive
point, but is hard to address without sacrificing the minimalism, simplicity,
and flexibility that catnip strives for. As a (perhaps unsatisfactory) remedy,
this documentation page contains many [examples](examples_index.md), many of
which implement common behaviors found in other compositors. Please do not be
afraid to copy and paste what you need to build your user config.

## Default User Config

Catnip ships with a default user config that is loaded if it fails to find any
valid user configs. This may be caused either by not being able to find any of
the paths listed above or if Lua threw an error while running the user config.

The default user config is not meant for daily driving and serves primarily as a
guaranteed loadable configuration. It focuses on providing enough for the user
to be able to debug / fix their configuration and
[reload](api_catnip.md#catnipreload) the Lua state.

Catnip embeds the default user config directly into the executable to ensure
that it is always available to the user. It can be printed to stdout using:

```bash
> catnip --default
{{ #include ../src/default_config.lua:1 }}
...
```

Below is the latest version of the default user config:

```lua
{{ #include ../src/default_config.lua }}
```
