# Space Rocks

Space Rocks is a 1996–1997 asteroid game originally built with Sense8
WorldToolKit. This repository contains the original C application and assets,
revived for modern macOS through the open WTK-NG compatibility runtime.

## Requirements

- macOS
- SDL2
- libjpeg
- WTK-NG checked out beside this repository as `../wtk-ng`

With Homebrew, the native dependencies are available as `sdl2` and
`jpeg-turbo`.

## Build and run

```sh
make
./rocks-ng -w
```

Override the runtime location when necessary:

```sh
make WTK_NG_DIR=/path/to/wtk-ng
```

See `readme.txt` for the original game documentation and controls.

## Status

The native compatibility port supports the scene graph, models, textures,
input, animation, collisions, overlays, and sound used by Space Rocks. Some
legacy WorldToolKit UI, hardware, networking, and spatial-audio facilities are
represented by compatibility stubs.
