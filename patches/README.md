# Falkon Build Patches

Patches for Falkon source code that are applied before building.
Everything under `falkon\src\` mirrors the upstream layout and is copied
over a fresh upstream checkout by `apply_falkon_patches.cmd`.

## Customizing Build Info

Edit these files to change the About page info:

1. `falkon\src\lib\other\aboutdialog.cpp` - About dialog (Help > About Falkon)
2. `falkon\src\lib\network\schemehandlers\falkonschemehandler.cpp` - about:falkon page
3. `falkon\src\lib\data\html\about.html` - about:falkon page template

## ProxySwitcher (proxypatched builds)

SwitchyOmega-like proxy profile switcher plugin. New code lives in
`falkon\src\plugins\ProxySwitcher\` (no upstream files touched inside).

Five upstream files are modified to wire it in. Search for `PROXYSWITCHER-PATCH`
comments when re-applying after an upstream update:

1. `falkon\src\plugins\CMakeLists.txt`
   - one added line: `add_subdirectory(ProxySwitcher)`
2. `falkon\src\lib\app\mainapplication.cpp` (function `setChromiumFlags`)
   - reads `Web-Proxy/webrtcProtect` setting and appends
     `--force-webrtc-ip-handling-policy=disable-non-proxy-udp`
3. `falkon\src\lib\app\datapaths.cpp` (function `init`, right after the
   `FALKON_PLUGIN_PATH` block)
   - prepends `<appdir>/../lib/plugins` to the plugin search path so the
     deployed zip finds its plugins without build-machine paths
   - related build-script change: `30_deploy_falkon.cmd` copies
     `%QT_CODEC%\plugins\*.dll` into `lib\plugins\` and `bin\plugins\`
     (bin\plugins keeps portable mode working too)
4. `falkon\src\lib\network\networkmanager.cpp` (function `loadSettings`)
   - while `Web-Proxy/Takeover=true` in the profile settings.ini, the
     preferences-page proxy configuration is NOT applied at all, so saving
     preferences can never override an active plugin profile
5. `falkon\src\lib\preferences\preferences.cpp` (function `loadSettings`)
   - while `Web-Proxy/Takeover=true`, all widgets of the "Proxy
     Configuration" tab are disabled and a hint label explains that the
     ProxySwitcher plugin currently owns the network configuration

## QtWebEngine patch (bypass rules for the application proxy)

`qtwebengine\src\core\net\proxy_config_service_qt.{h,cpp}` mirrors the Qt
module files (Qt 6.11.0). Applied with `apply_qtwebengine_patches.cmd`
(called automatically by `11_build_qtwebengine.cmd`) into
`C:\Qt\6.11.0\Src\qtwebengine`, then rebuild + `12_install_qtwebengine.cmd`.

Behavior: when Falkon runs with a QNetworkProxy application proxy
(= a ProxySwitcher profile is active), the patch also feeds the contents of
the environment variable `FALKON_PROXY_BYPASS` into Chromium's
ProxyBypassRules. The ProxySwitcher plugin sets that variable from the
active profile's "Bypass list" (comma separated; wildcards `*.example.com`,
IPs, CIDR `10.0.0.0/8` etc.). Changes are picked up live by the poll-based
ProxyConfigServiceQt, no browser restart needed.
Without the rebuilt QtWebEngineCore the variable is simply ignored (bypass
list is a no-op, everything else still works).

## Takeover / control handoff

- `Web-Proxy/Takeover` (in `<profile>/settings.ini`, written via Falkon's
  Settings class) is set to `true` by the plugin while any profile /
  direct / system mode is active; `false` when the plugin hands control
  back ("Disable takeover" menu entry, plugin disabled, or broken profile
  reference). On handoff the plugin calls `NetworkManager::loadSettings()`
  so the preferences-page configuration is re-applied immediately.
- Note: Falkon's real settings file is `<profile>/settings.ini`
  (portable: `bin\config\profiles\default\settings.ini`), NOT falkon.ini.
- Known Qt quirk handled in `ps_manager.cpp`: `QSettings::childGroups()`
  inside the Falkon process does not report the nested `Profiles/<name>`
  INI sections reliably; profile names are therefore parsed from
  `allKeys()` instead. Do not "simplify" this back to childGroups().

Plugin user data: `<profile>/extensions/proxyswitcher.ini`
(passwords stored in clear text in that ini - same policy as the built-in
Preferences proxy page).

Enable in: menu Extensions > Manage Extensions > "Proxy Switcher".
UI entry points:
- status bar routing icon (toggle in plugin settings, applies to new windows)
- navigation toolbar: right-click toolbar > Configure Toolbar > available
  item "Proxy Switcher" (same mechanism as AdBlock, AbstractButtonInterface)
- Extensions menu submenu
- plugin settings dialog (Manage profiles...)

## Applying

Run `C:\bd\_scripts\apply_falkon_patches.cmd` before building.
Or use `C:\bd\_scripts\99_build_falkon_only.cmd` which does it automatically.

## When Falkon source is updated

After pulling new Falkon source, re-run the build script - patches will be re-applied.
If upstream files changed, you may need to update the patched files in this directory
(keep the `PROXYSWITCHER-PATCH` markers). The ProxySwitcher plugin directory itself
is self-contained and normally survives upstream updates untouched.
