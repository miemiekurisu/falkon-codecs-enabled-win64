# Falkon Codecs-Enabled Win64 Build

Unofficial Windows x64 builds of [Falkon](https://www.falkon.org/) web browser with **proprietary codec support** enabled (H.264, AAC, MP3, etc.).

**This is an unofficial third-party build. Not endorsed by the Falkon project, KDE, or The Qt Company.**

## Build Info

| Field | Value |
|-------|-------|
| Build ID | v2026.05.16-codecs.1 |
| Builder | miemiekurisu |
| Qt Version | 6.11.0 |
| Falkon Version | 24.12.2 |
| Platform | Windows 10/11 x64 |
| Compiler | MSVC 2022 x64 |
| Codecs | proprietary_codecs, rtc_use_h264, ffmpeg_branding=Chrome |

## Downloads

| File | SHA256 |
|------|--------|
| [ZIP](https://github.com/miemiekurisu/falkon-codecs-enabled-win64/releases/download/v2026.05.16-codecs.1/falkon-codecs-win64-v2026.05.16-codecs.1.zip) | `312bb39cde9a15da2c0b922cec78f8ab9fc2bf24d1586191cecc91ce1ae1d3e5` |
| [7z](https://github.com/miemiekurisu/falkon-codecs-enabled-win64/releases/download/v2026.05.16-codecs.1/falkon-codecs-win64-v2026.05.16-codecs.1.7z) | `a7a7120e554351874cd8dc08ce3b8ef9e1350ccb7e9682e74bade672c5bb44ea` |

## Differences from upstream

- **Proprietary codecs**: H.264, AAC, MP3, etc. enabled in QtWebEngine
- **Windows theme**: Navigation buttons use `qproperty-multiIcon` with chrome-style PNGs for reliable rendering without system icon theme
- **Build info**: `about:falkon` page and About dialog display build metadata

## License

Falkon is free software released under the GNU General Public License version 3 or later.

This build includes third-party components (Qt, KDE Frameworks, OpenSSL, FFmpeg)
under their respective licenses. See [LICENSE](LICENSE) for full terms.

## Disclaimer

THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
See [LICENSE](LICENSE) for complete disclaimer of warranty and liability.

This build enables proprietary codecs. Patent licensing may apply
in certain jurisdictions. Use at your own risk.

No technical support is provided.
