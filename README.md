# Orbish

Audio looping plugin for macOS and Windows.

## Download

See [Releases](https://github.com/orfeoPreto/Orbish-GPL/releases) for pre-built installers.

**macOS**: Standalone app (.app), AU plugin (.component), VST3 plugin (.vst3)
**Windows**: Standalone app (.exe), VST3 plugin (.vst3)

## Building from Source

### Requirements

- CMake 3.22+
- JUCE 8.0.6+ (cloned automatically by CMake in CI, or provide locally)
- macOS: Xcode 14+
- Windows: Visual Studio 2022

### Build

```bash
git clone https://github.com/orfeoPreto/Orbish-GPL.git
cd Orbish-GPL
git clone --depth 1 --branch 8.0.6 https://github.com/juce-framework/JUCE.git

cmake -B build
cmake --build build --config Release
```

Binaries will be in `build/Orbish_artefacts/Release/`.

## License

This software is licensed under the [GNU General Public License v3](LICENSE.txt).

Copyright 2019-2026 Duke Quarcoo / EXU

## Third-Party

See [THIRD_PARTY_NOTICES.txt](THIRD_PARTY_NOTICES.txt) for third-party licenses.
