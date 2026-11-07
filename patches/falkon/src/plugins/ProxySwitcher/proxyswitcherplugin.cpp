/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#include "proxyswitcherplugin.h"
#include "ps_manager.h"
#include "ps_controlbutton.h"
#include "ps_managerdialog.h"
#include "pluginproxy.h"
#include "browserwindow.h"
#include "navigationbar.h"
#include "statusbar.h"
#include "mainapplication.h"
#include "../config.h"

#include <QMenu>

void ProxySwitcherPlugin::init(InitState state, const QString &settingsPath)
{
    m_manager = new PS_Manager(settingsPath);

    // Re-assert the stored selection after mainline loadSettings() has run
    m_manager->applyActiveProfile();

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &ProxySwitcherPlugin::mainWindowCreated);
    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, &ProxySwitcherPlugin::mainWindowDeleted);

    if (state == LateInitState) {
        const auto windows = mApp->windows();
        for (BrowserWindow* window : windows) {
            mainWindowCreated(window);
        }
    }
}

void ProxySwitcherPlugin::unload()
{
    if (mApp->isClosing()) {
        return;
    }

    // Remove control buttons before the manager they reference is gone
    for (auto it = m_buttons.cbegin(); it != m_buttons.cend(); ++it) {
        if (it.key() && it.value()) {
            it.key()->statusBar()->removeButton(it.value());
            it.key()->navigationBar()->removeToolButton(it.value());
            delete it.value();
        }
    }
    m_buttons.clear();

    if (m_manager) {
        // Hand network control back to the mainline preferences
        if (m_manager->activeProfile() != PS_ACTIVE_OFF) {
            m_manager->setActiveProfile(PS_ACTIVE_OFF);
        }
        delete m_manager;
        m_manager = nullptr;
    }
}

bool ProxySwitcherPlugin::testPlugin()
{
    // Require the version that the plugin was built with
    return (QString::fromLatin1(Qz::VERSION) == QLatin1String(FALKON_VERSION));
}

void ProxySwitcherPlugin::showSettings(QWidget *parent)
{
    if (!m_manager) {
        return;
    }

    QWidget* dialogParent = parent;
    if (!dialogParent) {
        const auto windows = mApp->windows();
        if (!windows.isEmpty()) {
            dialogParent = windows.first();
        }
    }

    auto* dialog = new PS_ManagerDialog(m_manager, dialogParent);
    dialog->open();
}

void ProxySwitcherPlugin::populateExtensionsMenu(QMenu *menu)
{
    if (!m_manager) {
        return;
    }

    QMenu* subMenu = menu->addMenu(tr("Proxy Switcher"));
    PS_ControlButton::buildProfileMenu(subMenu, m_manager, this,
                                       [this](const QString &name) { m_manager->setActiveProfile(name); },
                                       [this]() { showSettings(); });
}

void ProxySwitcherPlugin::mainWindowCreated(BrowserWindow *window)
{
    if (!m_manager || !window) {
        return;
    }

    auto* button = new PS_ControlButton(m_manager, [this, window]() { showSettings(window); }, window);

    if (m_manager->statusButtonVisible()) {
        window->statusBar()->addButton(button);
    }
    // Registered as a "Configure Toolbar" available item (like AdBlock does)
    window->navigationBar()->addToolButton(button);

    m_buttons[window] = button;
}

void ProxySwitcherPlugin::mainWindowDeleted(BrowserWindow *window)
{
    if (m_buttons.contains(window)) {
        PS_ControlButton* button = m_buttons.take(window);
        if (button) {
            window->statusBar()->removeButton(button);
            window->navigationBar()->removeToolButton(button);
        }
    }
}
