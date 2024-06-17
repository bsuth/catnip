# Default User Config

Catnip ships with a default [user config](getting_started_user_config.md) that
is loaded if it fails to load any other user configs. This may happen whenever
catnip is unable to find any other configs or if those configs threw an error
while loading.

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
