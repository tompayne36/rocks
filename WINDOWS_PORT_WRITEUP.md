# Space Rocks Windows revival

## Outcome

Space Rocks now builds and runs as a native 64-bit Windows application using
the WTK-NG compatibility runtime. A portable package includes the executable,
game assets, SDL2 and libjpeg DLLs, dependency licenses, and a double-click
launcher. MSYS2 is needed to build the game, but is not needed to play the
packaged version.

This work extends the recent macOS revival of the original 1996–1997 Sense8
WorldToolKit application.

## Repositories and local setup

The two links supplied initially both pointed to Rocks. The runtime repository
was found at `https://github.com/tompayne36/wtk-ng`.

The workspace did not contain a Git checkout, so both repositories were cloned
under `C:\Users\tom\projects\rocks`:

| Directory | Purpose |
| --- | --- |
| `source` | Space Rocks application source and original assets |
| `wtk-ng` | WorldToolKit compatibility runtime |
| `msys64` | Workspace-local MSYS2 build environment |

The sandbox denied access to the workspace during command execution. Approved
execution outside the sandbox was used to inspect the checkouts, install the
local build tools, compile, and run the game.

MSYS2 was extracted into the workspace rather than installed system-wide.
Its UCRT64 environment supplied the native Windows compiler and libraries.
No permanent system PATH change was made.

The versions used for validation were:

| Component | Version |
| --- | --- |
| GCC | 16.2.0, MSYS2 Rev4 |
| SDL2 | 2.32.10 |
| libjpeg-turbo | 3.2.0 |

## Source changes

### Rocks build

The original revived Makefile assumed macOS. `source/Makefile` was updated to:

- Produce `rocks-ng.exe` on Windows.
- Link Windows OpenGL with `-lopengl32`.
- Retain the macOS OpenGL framework branch and provide `-lGL` for other native
  platforms.
- Support compiler and `pkg-config` overrides.
- Keep preprocessing, compilation, and linking flags separate.
- Honor `LDFLAGS` and use the selected executable name for the run target.
- Remove both native executable names when cleaning.

SDL2's Windows `pkg-config` output included `-Dmain=SDL_main`. That flag was
filtered out so the application retains its normal C `main` entry point.
WTK-NG already handles SDL initialization through `SDL_SetMainReady`.

### WTK-NG portability

`wtk-ng/wtk_ng.c` used the macOS-specific `<OpenGL/gl.h>` header on every native
build. It now selects that header on Apple platforms and `<GL/gl.h>` elsewhere,
while retaining the Emscripten include branch.

The first Windows compile also exposed a header-order conflict. The classic
WTK header defines the coordinate-axis macros `X`, `Y`, and `Z`; Windows headers
use some of those names in declarations. Moving `wt.h` after SDL and the system
headers prevents those declarations from being altered by the WTK macros.
An unused `<sys/time.h>` include was removed.

`wtk-ng/Makefile` now supports a `PKG_CONFIG` override and also filters out SDL's
`main` renaming flag.

The game logic and compatibility API were otherwise left as they were.

### Portable packaging

The new `source/tools/package-windows.sh` runs in an MSYS2 UCRT64 or MINGW64
shell and creates `source/build/windows/SpaceRocks`.

It copies the executable, models, textures, sounds, configuration assets, and
documentation. It uses `ldd` to identify runtime DLL dependencies, including
transitive imports, and copies dependencies from the selected MinGW environment.
It rejects missing DLLs or a dependency on the MSYS runtime.

For this build, the required bundled DLLs were `SDL2.dll` and `libjpeg-8.dll`.
The package also includes the SDL2 and libjpeg-turbo licenses.

The generated `Play Space Rocks.cmd` launcher changes to its own directory
before running `rocks-ng.exe -w`. This lets the game find its assets when the
launcher is opened from Explorer or invoked from another directory.
A short `README-Windows.txt` explains launching and basic controls.

Windows build and packaging instructions were added to the README in each
repository.

## Build procedure

After updating MSYS2, install the following packages from its UCRT64 shell:

```sh
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-SDL2 \
  mingw-w64-ucrt-x86_64-libjpeg-turbo
```

With the two checkouts beside one another, run from the Rocks checkout:

```sh
make -C ../wtk-ng
make
./rocks-ng.exe -w
bash tools/package-windows.sh
```

Both checkouts must include the Windows changes described above. Clean both
projects before switching compiler or platform.

## Validation

Both projects compiled successfully with the native UCRT64 toolchain. The
compiler reported warnings in existing code; those warnings were not treated
as evidence of a warning-free build.

Bounded runtime checks used WTK-NG's existing diagnostic environment variables:

- A 120-frame startup run.
- A 180-frame run injecting firing, viewpoint changes, texture selection, and
  forward thrust.
- A 240-frame run of the packaged launcher injecting firing, an explosion,
  alien creation, and performance reporting.

The packaged launcher was invoked from outside the game directory with PATH
limited to Windows system directories. It exited successfully, demonstrating
that the tested package did not rely on MSYS2 being on PATH.

WTK-NG captured rendered frames during these runs. Visual inspection confirmed
the textured starfield, logo overlay, cockpit and external ship views, and
asteroids. Runtime output showed universe initialization, asteroid creation,
and performance statistics. No audio-initialization failure appeared in the
logs; this was not a listening test of sound quality.

`git diff --check` passed in both checkouts. The final ZIP passed its integrity
check and contained 105 entries, including the executable, required DLLs,
launcher, key game assets, and dependency licenses.

These were Windows build and smoke checks, not an exhaustive gameplay test.
macOS and Emscripten were not rebuilt during this work.

## Deliverables and status

All paths below are relative to `C:\Users\tom\projects\rocks`:

| Artifact | Location |
| --- | --- |
| Portable game folder | `source/build/windows/SpaceRocks` |
| Double-click launcher | `source/build/windows/SpaceRocks/Play Space Rocks.cmd` |
| Shareable ZIP | `SpaceRocks-Windows-x64.zip` |
| Build and runtime log | `windows-validation.log` |
| Standalone launcher log | `windows-portable.log` |
| Render captures | `windows-startup.png`, `windows-gameplay.png`, `windows-portable.png` |
| Local validation scripts | `validate-windows.sh`, `validate-package.ps1` |

At the end of the initial Windows port, the changes were local and uncommitted.
The packaged game still inherits WTK-NG's existing compatibility limitations for
legacy UI, specialized hardware, networking, and spatial-audio facilities.

## GitHub Actions follow-up (October 7, 2026)

Native build workflows were added to both repositories for Windows x64, macOS
Apple Silicon, and macOS Intel. They run on pushes and pull requests, and also
support manual execution. Each job uses a standard GitHub-hosted runner, builds
with the platform dependencies, and uploads build artifacts for 14 days.

The Rocks workflow pins its WTK-NG dependency to an immutable commit containing
the Windows fixes. Its Windows artifact is a portable game folder. The macOS
artifacts contain an unsigned executable, game assets, and a launcher, and
require Homebrew SDL2 and jpeg-turbo on the matching processor architecture.

The new `tools/package-macos.sh` creates the macOS archives. Runtime artifacts
from the WTK-NG workflow contain the static library, header, and README.
The workflows check compilation and packaging; interactive macOS graphics,
controls, and sound still need validation on a Mac.
