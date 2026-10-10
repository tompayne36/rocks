#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary="$project_dir/rocks-ng"
dist_dir="$project_dir/dist"
app="$dist_dir/Space Rocks.app"
contents="$app/Contents"
macos="$contents/MacOS"
frameworks="$contents/Frameworks"
resources="$contents/Resources"

if [ ! -x "$binary" ]; then
    echo "error: $binary has not been built" >&2
    exit 1
fi

arch=$(file "$binary" | sed -n 's/.*Mach-O 64-bit executable \([^ ]*\).*/\1/p')
if [ -z "$arch" ]; then
    arch=unknown
fi
archive="$dist_dir/SpaceRocks-macOS-$arch.zip"

rm -rf "$app"
rm -f "$archive"
mkdir -p "$macos" "$frameworks" "$resources"

cp "$binary" "$macos/rocks-ng"

cat > "$contents/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "https://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key><string>English</string>
    <key>CFBundleDisplayName</key><string>Space Rocks</string>
    <key>CFBundleExecutable</key><string>SpaceRocks</string>
    <key>CFBundleIdentifier</key><string>com.tompayne.spacerocks</string>
    <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
    <key>CFBundleName</key><string>Space Rocks</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>1.0</string>
    <key>CFBundleVersion</key><string>1</string>
    <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
PLIST

cat > "$macos/SpaceRocks" <<'LAUNCHER'
#!/bin/sh
set -eu
contents=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$contents/Resources"
exec "$contents/MacOS/rocks-ng" -w "$@"
LAUNCHER
chmod 755 "$macos/SpaceRocks" "$macos/rocks-ng"

# The compatibility runtime searches the current directory for legacy assets.
find "$project_dir" -maxdepth 1 -type f \( \
    -iname '*.flt' -o -iname '*.nff' -o -iname '*.tga' -o \
    -iname '*.jpg' -o -iname '*.wav' -o -iname '*.mat' -o \
    -iname '*.txt' -o -iname '*.bmp' -o -name 'LIGHTS' -o -name 'UNI' \
\) -exec cp {} "$resources/" \;

copy_dependency() {
    dependency=$1
    case "$dependency" in
        /System/*|/usr/lib/*|@*) return 0 ;;
    esac

    if [ ! -f "$dependency" ]; then
        echo "error: required library not found: $dependency" >&2
        exit 1
    fi

    name=$(basename "$dependency")
    cp "$dependency" "$frameworks/$name"
    chmod u+w "$frameworks/$name"
    install_name_tool -change "$dependency" "@executable_path/../Frameworks/$name" "$macos/rocks-ng"
    install_name_tool -id "@rpath/$name" "$frameworks/$name"
}

otool -L "$binary" | tail -n +2 | awk '{print $1}' | while IFS= read -r dependency; do
    copy_dependency "$dependency"
done

# Rewrite any non-system dependencies between copied libraries as well.
for library in "$frameworks"/*.dylib; do
    [ -e "$library" ] || continue
    otool -L "$library" | tail -n +2 | awk '{print $1}' | while IFS= read -r dependency; do
        case "$dependency" in
            /System/*|/usr/lib/*|@*) continue ;;
        esac
        name=$(basename "$dependency")
        if [ ! -f "$frameworks/$name" ]; then
            cp "$dependency" "$frameworks/$name"
            chmod u+w "$frameworks/$name"
            install_name_tool -id "@rpath/$name" "$frameworks/$name"
        fi
        install_name_tool -change "$dependency" "@loader_path/$name" "$library"
    done
done

for library in "$frameworks"/*.dylib; do
    [ -e "$library" ] || continue
    codesign --force --sign - "$library"
done
codesign --force --sign - "$macos/rocks-ng"
codesign --force --deep --sign - "$app"
codesign --verify --deep --strict "$app"

ditto -c -k --sequesterRsrc --keepParent "$app" "$archive"

echo "Created $archive"
echo "Architecture: $arch"
echo "Assets: $(find "$resources" -type f | wc -l | tr -d ' ')"
