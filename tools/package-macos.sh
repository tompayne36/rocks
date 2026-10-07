#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
if [[ "$(uname -s)" != Darwin ]]; then
    echo 'Run this script on macOS after building rocks-ng.' >&2
    exit 1
fi

destination=build/macos/SpaceRocks
mkdir -p "$destination"
cp rocks-ng "$destination/"
find . -maxdepth 1 -type f \( -iname '*.tga' -o -iname '*.jpg' \
    -o -iname '*.bmp' -o -iname '*.nff' -o -iname '*.flt' \
    -o -iname '*.mat' -o -iname '*.wav' -o -iname '*.txt' \) \
    -exec cp '{}' "$destination/" \;
cp UNI LIGHTS README.md "$destination/"
cat > "$destination/Play Space Rocks.command" <<'EOF'
#!/bin/bash
cd "$(dirname "$0")" || exit 1
exec ./rocks-ng -w "$@"
EOF
chmod +x "$destination/Play Space Rocks.command"
cat > "$destination/README-macOS.txt" <<'EOF'
Space Rocks macOS build

This is an unsigned build archive, with game assets and a launcher.
It requires Homebrew SDL2 and jpeg-turbo for the same processor architecture:
    brew install sdl2 jpeg-turbo

From a terminal in this folder, run:
    bash "Play Space Rocks.command"

Use the arm64 build on Apple Silicon, or x86_64 on Intel Macs.
See readme.txt for game controls.
EOF
# tar preserves executable permissions when downloaded through Actions.
tar -czf "build/macos/SpaceRocks-macos-$(uname -m).tar.gz" \
    -C build/macos SpaceRocks
echo "macOS build archive: build/macos/SpaceRocks-macos-$(uname -m).tar.gz"
