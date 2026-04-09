#!/usr/bin/env bash
# Phase 9: Filesystem Reorganization
# Moves Source/ files into subdirectories matching Xcode project groups.
#
# USAGE:
#   1. Close Xcode
#   2. cd /Users/orfeopreto/Dev/Orbish
#   3. bash Scripts/migrate-phase9.sh
#   4. Reopen Xcode
#
# ROLLBACK: git checkout . && git clean -fd Source/

set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$(pwd)"
SRC="$ROOT/Source"
PBXPROJ="$ROOT/Builds/MacOSX/Orbish.xcodeproj/project.pbxproj"

echo "=== Phase 9: Filesystem Reorganization ==="
echo "Root: $ROOT"
echo ""

# ── Step 1: Create directory structure ──────────────────────────────────────
echo "Creating directories..."
mkdir -p "$SRC/AudioEngine"
mkdir -p "$SRC/AudioEngine/Tracks"
mkdir -p "$SRC/AudioEngine/Sync"
mkdir -p "$SRC/AudioEngine/Utility"
mkdir -p "$SRC/ApplicationView"
mkdir -p "$SRC/ApplicationView/OpenGLComponents"
mkdir -p "$SRC/ApplicationView/Tracks"
mkdir -p "$SRC/ApplicationView/InfoAndControl/Info"
mkdir -p "$SRC/ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl"
mkdir -p "$SRC/ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl"
mkdir -p "$SRC/ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl"
mkdir -p "$SRC/ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl"
mkdir -p "$SRC/ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl"
mkdir -p "$SRC/ApplicationView/Header"

# ── Step 2: Move files with git mv ─────────────────────────────────────────
echo "Moving files..."

move_file() {
    local file="$1"
    local dest="$2"
    if [ -f "$SRC/$file" ]; then
        git mv "$SRC/$file" "$SRC/$dest/$file"
    else
        echo "  WARNING: $file not found, skipping"
    fi
}

# AudioEngine (root)
for f in PluginProcessor.cpp PluginProcessor.h Context.h MidiProcessor.cpp MidiProcessor.h \
         ParameterDefinitions.h ParameterDefinitions.cpp ProcessorState.h \
         AllocatorThread.h AllocatorThread.cpp \
         PluginProcessorCommands.cpp PluginProcessorLifecycle.cpp \
         ClickGenerator.h PluginProcessorEvents.cpp \
         TrackEventHandler.h; do
    move_file "$f" "AudioEngine"
done

# AudioEngine/Tracks
for f in Track.cpp Track.h Loop.cpp Loop.h Layer.cpp Layer.h \
         TrackGroup.cpp TrackGroup.h TrackLoader.cpp TrackLoader.h \
         Track.fwd.h; do
    move_file "$f" "AudioEngine/Tracks"
done

# AudioEngine/Sync
for f in InternalSynchronizer.cpp InternalSynchronizer.h Synchronizer.h \
         HostSynchronizer.h HostSynchronizer.cpp Realignment.cpp Realignment.h; do
    move_file "$f" "AudioEngine/Sync"
done

# AudioEngine/Utility
for f in GainModifier.h GainModifier.cpp DataExchange.h DataExchange.cpp \
         TimeStretchRubberBand.h; do
    move_file "$f" "AudioEngine/Utility"
done

# ApplicationView (root)
for f in ImageViewport.cpp ImageViewport.h OrbishLevelMeter.h OrbishLevelMeter.cpp \
         OrbishSlider.cpp OrbishSlider.h Observer.h \
         Blinker.hpp CommandIDs.h LoopComponent.h CustomButton.cpp CustomButton.h \
         MainMenu.cpp MainMenu.h MenuManager.h PluginEditor.cpp PluginEditor.h \
         SettingsPage.cpp SettingsPage.h OrbishLookAndFeel.cpp OrbishLookAndFeel.h; do
    move_file "$f" "ApplicationView"
done
# Note: Label.hpp and UIStateGuard.hpp live in /exu/, not Source/ — no move needed

# ApplicationView/OpenGLComponents
for f in OpenGLAudioMeter.cpp OpenGLAudioMeter.h OpenGLAudioThumbnail.cpp \
         OpenGLAudioThumbnail.h OpenGLClickWitness.cpp OpenGLClickWitness.h \
         OpenGLComponent.cpp OpenGLComponent.h OpenGLComponentContainer.cpp \
         OpenGLComponentContainer.h OpenGLComponents.h Shader.cpp Shader.h; do
    move_file "$f" "ApplicationView/OpenGLComponents"
done

# ApplicationView/Tracks
for f in LoopComponent.cpp TrackComponent.cpp TrackComponent.h TrackArea.cpp TrackArea.h; do
    move_file "$f" "ApplicationView/Tracks"
done

# ApplicationView/InfoAndControl (root files)
for f in InfoAndControlArea.cpp InfoAndControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl"
done

# ApplicationView/InfoAndControl/Info
for f in InfoArea.cpp InfoArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Info"
done

# ApplicationView/InfoAndControl/Control (root files)
for f in ControlArea.cpp ControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control"
done

# ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl
for f in GroupControlArea.cpp GroupControlArea.h ThumbnailAndGroupArea.cpp \
         ThumbnailAndGroupArea.h ThumbnailArea.cpp ThumbnailArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl"
done

# ApplicationView/InfoAndControl/Control/ButtonControl (root files)
for f in ButtonControlArea.cpp ButtonControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control/ButtonControl"
done

# ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl
for f in InputControlArea.cpp InputControlArea.h OutputControlArea.cpp OutputControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl"
done

# ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl
for f in ModeAndNavigationControlArea.cpp ModeAndNavigationControlArea.h \
         ModeControlArea.cpp ModeControlArea.h \
         NavigationControlArea.cpp NavigationControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl"
done

# ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl
for f in GlobalControlArea.cpp GlobalControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl"
done

# ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl
for f in TransportControlArea.cpp TransportControlArea.h; do
    move_file "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl"
done

# ApplicationView/Header
for f in HeaderArea.cpp HeaderArea.h; do
    move_file "$f" "ApplicationView/Header"
done

# Orbish.h stays at Source/ root (no move needed)

echo ""
echo "File moves complete."

# ── Step 3: Update pbxproj file references ─────────────────────────────────
echo "Updating pbxproj file references..."
cp "$PBXPROJ" "$PBXPROJ.bak"

# Helper: update a SOURCE_ROOT file reference path
# Usage: update_srcroot_path "OldFile.cpp" "NewSubdir"
update_srcroot_path() {
    local file="$1"
    local subdir="$2"
    sed -i '' "s|path = ../../Source/${file};|path = ../../Source/${subdir}/${file};|g" "$PBXPROJ"
}

# Helper: convert a group-relative file reference to SOURCE_ROOT
# Usage: convert_group_to_srcroot "File.cpp" "NewSubdir"
convert_group_to_srcroot() {
    local file="$1"
    local subdir="$2"
    # Match: path = File.cpp; sourceTree = "<group>";
    # Replace with: name = File.cpp; path = ../../Source/Subdir/File.cpp; sourceTree = SOURCE_ROOT;
    sed -i '' "s|path = ${file}; sourceTree = \"<group>\";|name = ${file}; path = ../../Source/${subdir}/${file}; sourceTree = SOURCE_ROOT;|g" "$PBXPROJ"
}

# Helper: convert a group-relative file with ../../Source path to SOURCE_ROOT
convert_group_srcpath_to_srcroot() {
    local file="$1"
    local subdir="$2"
    sed -i '' "s|path = ../../Source/${file}; sourceTree = \"<group>\";|path = ../../Source/${subdir}/${file}; sourceTree = SOURCE_ROOT;|g" "$PBXPROJ"
}

# --- AudioEngine (root) ---
for f in PluginProcessor.cpp PluginProcessor.h Context.h MidiProcessor.cpp MidiProcessor.h; do
    update_srcroot_path "$f" "AudioEngine"
done
for f in ParameterDefinitions.h ParameterDefinitions.cpp ProcessorState.h \
         AllocatorThread.h AllocatorThread.cpp \
         PluginProcessorCommands.cpp PluginProcessorLifecycle.cpp \
         ClickGenerator.h PluginProcessorEvents.cpp; do
    convert_group_to_srcroot "$f" "AudioEngine"
done

# --- AudioEngine/Tracks ---
for f in Track.cpp Track.h Loop.cpp Loop.h Layer.cpp Layer.h \
         TrackGroup.cpp TrackGroup.h TrackLoader.cpp TrackLoader.h; do
    update_srcroot_path "$f" "AudioEngine/Tracks"
done

# --- AudioEngine/Sync ---
for f in InternalSynchronizer.cpp InternalSynchronizer.h Synchronizer.h \
         HostSynchronizer.h HostSynchronizer.cpp Realignment.cpp Realignment.h; do
    update_srcroot_path "$f" "AudioEngine/Sync"
done

# --- AudioEngine/Utility ---
for f in GainModifier.h GainModifier.cpp DataExchange.h; do
    update_srcroot_path "$f" "AudioEngine/Utility"
done
# TimeStretchRubberBand.h uses sourceTree = "<group>" with ../../Source/ path
convert_group_srcpath_to_srcroot "TimeStretchRubberBand.h" "AudioEngine/Utility"

# --- ApplicationView (root) ---
for f in ImageViewport.cpp ImageViewport.h OrbishLevelMeter.h OrbishLevelMeter.cpp \
         OrbishSlider.cpp OrbishSlider.h Observer.h \
         Blinker.hpp CommandIDs.h LoopComponent.h CustomButton.cpp CustomButton.h \
         MainMenu.cpp MainMenu.h MenuManager.h PluginEditor.cpp PluginEditor.h \
         SettingsPage.cpp SettingsPage.h OrbishLookAndFeel.cpp OrbishLookAndFeel.h; do
    update_srcroot_path "$f" "ApplicationView"
done
# Note: Label.hpp and UIStateGuard.hpp are in /exu/ — pbxproj already correct

# --- ApplicationView/OpenGLComponents ---
for f in OpenGLAudioMeter.cpp OpenGLAudioMeter.h OpenGLAudioThumbnail.cpp \
         OpenGLAudioThumbnail.h OpenGLClickWitness.cpp OpenGLClickWitness.h \
         OpenGLComponent.cpp OpenGLComponent.h OpenGLComponentContainer.cpp \
         OpenGLComponentContainer.h OpenGLComponents.h Shader.cpp Shader.h; do
    update_srcroot_path "$f" "ApplicationView/OpenGLComponents"
done

# --- ApplicationView/Tracks ---
for f in LoopComponent.cpp TrackComponent.cpp TrackComponent.h TrackArea.cpp TrackArea.h; do
    update_srcroot_path "$f" "ApplicationView/Tracks"
done

# --- ApplicationView/InfoAndControl ---
for f in InfoAndControlArea.cpp InfoAndControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl"
done

# --- ApplicationView/InfoAndControl/Info ---
for f in InfoArea.cpp InfoArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Info"
done

# --- ApplicationView/InfoAndControl/Control ---
for f in ControlArea.cpp ControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control"
done

# --- ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl ---
for f in GroupControlArea.cpp GroupControlArea.h ThumbnailAndGroupArea.cpp \
         ThumbnailAndGroupArea.h ThumbnailArea.cpp ThumbnailArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl"
done

# --- ApplicationView/InfoAndControl/Control/ButtonControl ---
for f in ButtonControlArea.cpp ButtonControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control/ButtonControl"
done

# --- ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl ---
for f in InputControlArea.cpp InputControlArea.h OutputControlArea.cpp OutputControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl"
done

# --- ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl ---
for f in ModeAndNavigationControlArea.cpp ModeAndNavigationControlArea.h \
         ModeControlArea.cpp ModeControlArea.h \
         NavigationControlArea.cpp NavigationControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl"
done

# --- ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl ---
for f in GlobalControlArea.cpp GlobalControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl"
done

# --- ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl ---
for f in TransportControlArea.cpp TransportControlArea.h; do
    update_srcroot_path "$f" "ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl"
done

# --- ApplicationView/Header ---
for f in HeaderArea.cpp HeaderArea.h; do
    update_srcroot_path "$f" "ApplicationView/Header"
done

echo "pbxproj file references updated."

# ── Step 4: Update header search paths in pbxproj ──────────────────────────
echo "Updating header search paths..."

# Write a Python helper script for reliable multiline regex replacement
cat > /tmp/update_hsp.py << 'PYEOF'
import re, sys

pbxproj_path = sys.argv[1]
with open(pbxproj_path, 'r') as f:
    content = f.read()

# The replacement value — an array of all Source subdirectories
T = "\t\t\t\t\t"
dirs = [
    "$(SRCROOT)/../../Source",
    "$(SRCROOT)/../../Source/AudioEngine",
    "$(SRCROOT)/../../Source/AudioEngine/Tracks",
    "$(SRCROOT)/../../Source/AudioEngine/Sync",
    "$(SRCROOT)/../../Source/AudioEngine/Utility",
    "$(SRCROOT)/../../Source/ApplicationView",
    "$(SRCROOT)/../../Source/ApplicationView/OpenGLComponents",
    "$(SRCROOT)/../../Source/ApplicationView/Tracks",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Info",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control/ButtonControl",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl",
    "$(SRCROOT)/../../Source/ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl",
    "$(SRCROOT)/../../Source/ApplicationView/Header",
]
items = ",\n".join(T + '"' + d + '"' for d in dirs)
new_value = "(\n" + items + ",\n\t\t\t\t)"

count = 0

# Pattern 1: simple string form
#   "USER_HEADER_SEARCH_PATHS[arch=*]" = "$(SRCROOT)/../../Source";
p1 = r'"USER_HEADER_SEARCH_PATHS\[arch=\*\]"\s*=\s*"\$\(SRCROOT\)/\.\./\.\./Source"\s*;'
content, n = re.subn(p1, '"USER_HEADER_SEARCH_PATHS[arch=*]" = ' + new_value + ';', content)
count += n

# Pattern 2: broken array form from earlier edit
#   "USER_HEADER_SEARCH_PATHS[arch=*]" = ( "$(", "SRCROOT)/../../", Source, );
p2 = r'"USER_HEADER_SEARCH_PATHS\[arch=\*\]"\s*=\s*\(\s*"\$\(",\s*"SRCROOT\)/\.\./\.\./",\s*Source,\s*\)\s*;'
content, n = re.subn(p2, '"USER_HEADER_SEARCH_PATHS[arch=*]" = ' + new_value + ';', content)
count += n

with open(pbxproj_path, 'w') as f:
    f.write(content)

print(f"  Replaced {count} USER_HEADER_SEARCH_PATHS entries")
PYEOF

python3 /tmp/update_hsp.py "$PBXPROJ"
rm -f /tmp/update_hsp.py

echo "Header search paths updated."

# ── Step 5: Remove stale copies in Builds/MacOSX/ ──────────────────────────
echo "Removing stale Builds/MacOSX/ copies..."
for f in AllocatorThread.cpp AllocatorThread.h ParameterDefinitions.cpp ParameterDefinitions.h \
         ProcessorState.h ClickGenerator.h PluginProcessorCommands.cpp \
         PluginProcessorLifecycle.cpp PluginProcessorEvents.cpp; do
    rm -f "$ROOT/Builds/MacOSX/$f"
done

# ── Step 6: Update test Makefile ────────────────────────────────────────────
echo "Updating test Makefile..."
MAKEFILE="$ROOT/Tests/Makefile"
if [ -f "$MAKEFILE" ]; then
    sed -i '' 's|-I$(ORBISH_ROOT)/Source \\|-I$(ORBISH_ROOT)/Source \\\
    -I$(ORBISH_ROOT)/Source/AudioEngine \\\
    -I$(ORBISH_ROOT)/Source/AudioEngine/Tracks \\\
    -I$(ORBISH_ROOT)/Source/AudioEngine/Sync \\\
    -I$(ORBISH_ROOT)/Source/AudioEngine/Utility \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/OpenGLComponents \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/Tracks \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Info \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control/ThumbnailAndGroupControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control/ButtonControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control/ButtonControl/OutputInputControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control/ButtonControl/ModeAndNavigationControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control/ButtonControl/GlobalControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/InfoAndControl/Control/ButtonControl/TransportControl \\\
    -I$(ORBISH_ROOT)/Source/ApplicationView/Header \\|' "$MAKEFILE"
    echo "Makefile updated."
else
    echo "WARNING: Makefile not found at $MAKEFILE"
fi

# ── Step 7: Summary ────────────────────────────────────────────────────────
echo ""
echo "=== Migration complete ==="
echo ""
echo "Moved files:"
find "$SRC" -mindepth 2 -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) | wc -l | xargs echo " "
echo "files into subdirectories."
echo ""
echo "Remaining at Source/ root:"
ls "$SRC"/*.h "$SRC"/*.cpp "$SRC"/*.hpp 2>/dev/null || echo "  (none — all moved)"
echo ""
echo "Next steps:"
echo "  1. Open Xcode"
echo "  2. Build (Cmd+B)"
echo "  3. If build succeeds: git add -A && git commit"
echo "  4. Run tests: cd Tests && make clean && make"
echo ""
echo "To rollback: git checkout . && git clean -fd Source/"
