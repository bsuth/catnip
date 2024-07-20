# Overview

Catnip is a [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots) based
[Wayland](https://wayland.freedesktop.org/) compositor that allows users to
customize their desktop via [Lua](https://www.lua.org/).

## The (Anti) Pitch

Catnip is not meant for users looking for an out-of-the-box functioning
compositor. It is meant for extremists (like myself) who enjoy wasting copious
amounts of time to customize their desktop for a deeply unique and personalized
experience.

In particular, catnip strives to be:

### Minimalistic

Catnip does not provide any default behavior, _everything must be defined by the
user_. This keeps catnip lean, unobstructive, and explicit. However, it also
means that catnip is verbose. The user may have to spend considerable time
defining how the compositor should function.

### Powerful

Catnip makes very few assumptions on what it should handle internally. Instead,
it delegates most of the implementation effort to the user by exposing a low
level API, allowing for complete control over compositor behavior.

### Stable

Due to the extremity of catnip's minimalism, the need to make changes (and in
particular, breaking changes) should be quite rare once v1.0 is reached.
Additionally, since much of the compositor behavior is defined by the user,
breaking changes in catnip can be destructive, even potentially rendering the
compositor unusable. For this reason, catnip takes great strides to ensure users
are given proper time and warning before breaking changes are rolled out.

***NOTE**: Catnip should still be considered beta before v1.0. Breaking changes
may happen between minor versions until v1.0 is reached.

## Similar Projects

Catnip is not for everybody (and probably shouldn't be). There are plenty of
other projects that will perfectly suffice for most users without requiring a
large time investment and deep understanding of the compositor:

- [hyprland](https://hyprland.org/)
- [sway](https://swaywm.org/)
- [wayfire](https://wayfire.org/)
- [qtile](https://github.com/qtile/qtile)
- [river](https://codeberg.org/river/river)
