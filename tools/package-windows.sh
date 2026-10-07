#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
if [[ "${OS:-}" != Windows_NT || -z "${MINGW_PREFIX:-}" ]]; then
    echo 'Run this script in an MSYS2 UCRT64 or MINGW64 shell.' >&2
    exit 1
fi
if [[ ! -f rocks-ng.exe ]]; then
    echo 'Build rocks-ng.exe with make first.' >&2
    exit 1
fi

destination=build/windows/SpaceRocks
mkdir -p "$destination"
cp rocks-ng.exe "$destination/"
# ldd includes transitive imports, so GCC runtime DLLs are included when needed.
dependencies=$(ldd ./rocks-ng.exe)
if [[ "$dependencies" == *'not found'* || "$dependencies" == *'msys-2.0.dll'* ]]; then
    printf 'Unexpected or missing DLL dependency:\n%s\n' "$dependencies" >&2
    exit 1
fi
while read -r name arrow path remainder; do
    case "$path" in
        "$MINGW_PREFIX"/bin/*) cp "$path" "$destination/" ;;
    esac
done <<< "$dependencies"

find . -maxdepth 1 -type f \( -iname '*.tga' -o -iname '*.jpg' \
    -o -iname '*.bmp' -o -iname '*.nff' -o -iname '*.flt' \
    -o -iname '*.mat' -o -iname '*.wav' -o -iname '*.txt' \) \
    -exec cp '{}' "$destination/" \;
cp UNI LIGHTS README.md "$destination/"
mkdir -p "$destination/licenses"
cp -R "$MINGW_PREFIX/share/licenses/SDL2" \
    "$MINGW_PREFIX/share/licenses/libjpeg-turbo" "$destination/licenses/"
printf '@echo off\r\ncd /d "%%~dp0"\r\nrocks-ng.exe -w %%*\r\n' > "$destination/Play Space Rocks.cmd"
cat > "$destination/README-Windows.txt" <<'EOF'
Space Rocks for Windows (64-bit)

Double-click "Play Space Rocks.cmd" to play.
Keep the executable, DLLs, and game assets together in this folder.
MSYS2 is not required to play. A working OpenGL graphics driver is required.

Space or left mouse button: fire
Arrow keys: pitch/roll
F or right mouse button: accelerate
B or middle mouse button: brake
V: switch viewpoint
P (lowercase): pause
Q (lowercase): quit

See readme.txt for the original game documentation.
Third-party library licenses are in the licenses folder.
EOF
echo "Portable Windows game: $destination"
