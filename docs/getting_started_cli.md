# CLI

Catnip ships as a standalone self-contained executable. It does not require any
additional files to run, as the [LuaCATS annotations](other_luacats.md) and
[default user config](other_default_user_config.md) are embedded directly into
the executable itself (shared libraries are still required by default unless
catnip is linked statically when building).

The available CLI options are documented below.

- [`-h, --help`](#-h---help)
- [`-v, --version`](#-v---version)
- [`-c, --config`](#-c---config-file)
- [`-l, --loglevel`](#-l---loglevel-level)
- [`-t, --types`](#-t---types)
- [`-d, --default`](#-d---default)

### `-h, --help`

Prints the help text and exits.

```
> catnip --help
usage: catnip [OPTION]
  -c, --config FILE     specify lua config path
  -l, --loglevel LEVEL  specify log level, must be one of:
                        silent, error, warning, info, debug
  -t, --types           print LuaCATS annotations and exit
  -d, --default         print default user config and exit
  -v, --version         print help and exit
  -h, --help            print version and exit
```

### `-v, --version`

Prints the version and exits.

```
> catnip --version
0.1.0
```

### `-c, --config FILE`

Specify the path to the Lua config file to use. This may be either an absolute
path or a path relative to the directory where the `catnip` executable is run.

```
> catnip --config ~/dots/catnip/init.lua
```

If unspecified, catnip will by default check the following paths (in the given order):

1. `$XDG_CONFIG_HOME/catnip/init.lua`
1. `~/.config/catnip/init.lua`

### `-l, --loglevel LEVEL`

Specifies the granularity for logging. The granularity from least to most
verbose is given as follows:

1. `silent`
1. `error`
1. `warning`
1. `info`
1. `debug`

```
> catnip --loglevel info
```

By default, catnip uses `--loglevel error`.

### `-t, --types`

Prints the [LuaCATS annotations](other_luacats.md) and exits.

```
> catnip --types
{{ #include ../src/luacats.lua:1 }}
...
```

### `-d, --default`

Prints the [default user config](other_default_user_config.md) and exits.

```
> catnip --default
{{ #include ../src/default_config.lua:1 }}
...
```
