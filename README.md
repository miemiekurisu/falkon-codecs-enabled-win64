# Falkon Codecs-Enabled Win64 Build

Unofficial Windows x64 builds of [Falkon](https://www.falkon.org/) web browser with **proprietary codec support** enabled (H.264, AAC, MP3, etc.).

**This is an unofficial third-party build. Not endorsed by the Falkon project, KDE, or The Qt Company.**

## Build Info

| Field | Value |
|-------|-------|
| Build ID | v26.11.71-codecs.1.proxypatched |
| Builder | miemiekurisu |
| Qt Version | 6.11.0 |
| Falkon Version | 26.11.71 |
| Platform | Windows 10/11 x64 |
| Compiler | MSVC 2022 x64 |
| Codecs | proprietary_codecs, rtc_use_h264, ffmpeg_branding=Chrome |

## Downloads

| File | SHA256 |
|------|--------|
| [ZIP](https://github.com/miemiekurisu/falkon-codecs-enabled-win64/releases/download/v26.11.71-codecs.1.proxypatched/falkon-codecs-win64-v26.11.71-codecs.1.proxypatched.zip) | `bbe009aac96bfe93797578d4650ebd8f2ed8ea79db8985ecd9a69209a5986d55` |
| [7z](https://github.com/miemiekurisu/falkon-codecs-enabled-win64/releases/download/v26.11.71-codecs.1.proxypatched/falkon-codecs-win64-v26.11.71-codecs.1.proxypatched.7z) | `9507a17515d4ba003b3b8772acd3cecc57ab1dd9dc1c02d2cfb6ca1acc7a52a4` |

## Differences from upstream

- **Proprietary codecs**: H.264, AAC, MP3, etc. enabled in QtWebEngine
- **ProxySwitcher plugin**: SwitchyOmega-style proxy profiles (system / direct / HTTP / SOCKS5 with auth) with instant live switching for all browser network traffic, status bar switcher and connectivity test. Enable in menu *Extensions > Manage Extensions*
- **WebRTC leak protection**: Optional WebRTC non-proxy UDP blocking (checkbox in ProxySwitcher settings, applied at startup)
- **Portable plugin discovery**: Extension plugins are discovered next to the executable (`lib/plugins` and `bin/plugins`), no build-machine paths required
- **Windows theme**: Navigation buttons use `qproperty-multiIcon` with chrome-style PNGs for reliable rendering without system icon theme
- **Build info**: `about:falkon` page and About dialog display build metadata

Known limitations of the proxy takeover: bypass lists and per-URL auto-switch rules
are not supported by the QtWebEngine application-proxy path; already established
keep-alive connections keep their original route until they close.

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
