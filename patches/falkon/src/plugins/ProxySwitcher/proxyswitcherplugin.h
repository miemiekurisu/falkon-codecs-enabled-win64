/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#ifndef PROXYSWITCHERPLUGIN_H
#define PROXYSWITCHERPLUGIN_H

#include "plugininterface.h"

#include <QObject>
#include <QHash>

class PS_Manager;
class PS_ControlButton;
class BrowserWindow;
class QMenu;

class ProxySwitcherPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.ProxySwitcher" FILE "proxyswitcher.json")

public:
    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
    void showSettings(QWidget *parent = nullptr) override;
    void populateExtensionsMenu(QMenu *menu) override;

private Q_SLOTS:
    void mainWindowCreated(BrowserWindow *window);
    void mainWindowDeleted(BrowserWindow *window);

private:
    PS_Manager *m_manager = nullptr;
    QHash<BrowserWindow*, PS_ControlButton*> m_buttons;
};

#endif // PROXYSWITCHERPLUGIN_H
