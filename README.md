# Wayland McWayface

This is a demonstration Wayland compositor that accompanies a series of blog
posts written for [drewdevault.com](https://drewdevault.com).

- **[Part 1: Hello wlroots](https://drewdevault.com/2018/02/17/Writing-a-Wayland-compositor-1.html)** (you are here)

## Compiling

```shell
mkdir build
cd build
meson ..
ninja
```

This will produce a binary named `mcwayface`, this is your compositor. Run it
without any arguments.
