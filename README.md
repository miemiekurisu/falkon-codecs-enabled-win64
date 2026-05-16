# Falkon Codecs-Enabled Win64 Build

Unofficial Windows x64 builds of [Falkon](https://www.falkon.org/) web browser with **proprietary codec support** enabled (H.264, AAC, MP3, etc.).

**This is an unofficial third-party build. Not endorsed by the Falkon project, KDE, or The Qt Company.**

## Build Info

| Field | Value |
|-------|-------|
| Build ID | v2026.07.70-codecs.1 |
| Builder | miemiekurisu |
| Qt Version | 6.11.0 |
| Falkon Version | 26.07.70 |
| Platform | Windows 10/11 x64 |
| Compiler | MSVC 2022 x64 |
| Codecs | proprietary_codecs, rtc_use_h264, ffmpeg_branding=Chrome |

## Downloads

| File | SHA256 |
|------|--------|
| [ZIP](https://github.com/miemiekurisu/falkon-codecs-enabled-win64/releases/download/v2026.07.70-codecs.1/falkon-codecs-win64-v2026.07.70-codecs.1.zip) | `ec4303d2ba98821b56d5d07f257b1b752a0efd328df40ef2a82e81b98a23b316` |
| [7z](https://github.com/miemiekurisu/falkon-codecs-enabled-win64/releases/download/v2026.07.70-codecs.1/falkon-codecs-win64-v2026.07.70-codecs.1.7z) | `a2c3ababc1826fd3d61d108dd9a4d062172346de4b77c69da46cf272ecf982e2` |

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
