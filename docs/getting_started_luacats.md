# LuaCATS

Catnip offers typing via [LuaCATS](https://luals.github.io/wiki/annotations/),
the annotation convention used by [luals](https://luals.github.io/). These types
are referenced throughout this site to provide additional documentation.

Catnip embeds the LuaCATS annotations directly into the executable to ensure the
user always has access to the proper types for the current version. It can be
printed to stdout using:

```bash
> catnip --types
{{ #include ../src/luacats.lua:1 }}
...
```

Below is the latest version of the types:

```lua
{{ #include ../src/luacats.lua }}
```
