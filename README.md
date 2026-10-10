# Space Rocks

Space Rocks is a 1996–1997 asteroid game originally built with Sense8
WorldToolKit. This repository contains the original C application and assets,
revived for modern macOS and Windows through the open WTK-NG compatibility runtime.

## macOS

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

## Create a self-contained macOS app

On a Mac, build an application bundle with its game assets and SDL2 and
libjpeg libraries:

```sh
make dist-macos
```

The ZIP is written to `dist/SpaceRocks-macOS-<architecture>.zip`. Unzip it and
open `Space Rocks.app`. The archive uses an ad-hoc signature, not an Apple
Developer ID signature; on first launch, macOS may require Control-clicking
the app and choosing **Open**. The filename identifies the build architecture.

## Windows (64-bit)

Install [MSYS2](https://www.msys2.org/) and open its **UCRT64** shell. Update
MSYS2 with `pacman -Syu`; if asked to close the shell, reopen UCRT64 and run
the update again. Install the native Windows compiler and dependencies:

```sh
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-SDL2 \
  mingw-w64-ucrt-x86_64-libjpeg-turbo
```

Check out [WTK-NG](https://github.com/tompayne36/wtk-ng) beside Rocks as
`../wtk-ng`. Both checkouts must include the Windows portability changes.
From the Rocks directory in the UCRT64 shell:

```sh
make -C ../wtk-ng
make
./rocks-ng.exe -w
```

Run from the Rocks directory so models, textures, and sounds can be found.
`WTK_NG_DIR=/path/to/wtk-ng` also works on Windows. When switching compiler
or platform, clean both projects before rebuilding.

To create a portable folder that runs without MSYS2 installed:

```sh
bash tools/package-windows.sh
```

The folder is `build/windows/SpaceRocks`. Double-click **Play Space Rocks.cmd**
to launch, or share the entire folder (including its DLLs and assets).
The launcher sets the working directory automatically. The Windows build
uses the native MinGW UCRT compiler, SDL2, libjpeg, and Windows OpenGL.

For a bounded startup check from the UCRT64 shell:

```sh
WTK_NG_FRAMES=120 ./rocks-ng.exe -w
WTK_NG_FRAMES=120 WTK_NG_START_KEY=' ' ./rocks-ng.exe -w
```

These exercise startup, gameplay, and firing; a working OpenGL display is required.

## Automated builds

The **Native builds** GitHub Actions workflow builds Windows x64 and macOS for
both Apple Silicon and Intel on every push and pull request. It also supports
manual runs from the Actions tab. WTK-NG is checked out at the immutable commit
specified by `WTK_NG_REF` in the workflow.

Successful runs have downloadable artifacts, retained for 14 days. The Windows
artifact is a portable game folder with its DLLs. The macOS artifacts contain
an unsigned executable, assets, and launcher in a tar archive; install
Homebrew `sdl2` and `jpeg-turbo` on the matching architecture before running.
These jobs verify compilation and packaging. Interactive graphics, input, and
sound still require testing on the target hardware.

## Status

The native compatibility port supports the scene graph, models, textures,
input, animation, collisions, overlays, and sound used by Space Rocks. Some
legacy WorldToolKit UI, hardware, networking, and spatial-audio facilities are
represented by compatibility stubs.
