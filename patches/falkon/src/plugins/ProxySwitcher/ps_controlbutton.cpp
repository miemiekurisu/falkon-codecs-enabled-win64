/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#include "ps_controlbutton.h"
#include "ps_manager.h"

#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QSize>
#include <QPixmap>
#include <QPainter>
#include <QColor>

// Adblock-like small status icon: routing arrow (client -> two endpoints),
// tinted when the plugin currently owns the network configuration.
static QIcon makeProxyIcon(bool takeoverActive)
{
    QPixmap pm(16, 16);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor main = takeoverActive ? QColor(0x2d, 0x7d, 0xd9) : QColor(0x88, 0x88, 0x88);
    QPen pen(main);
    pen.setWidthF(1.3);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);

    // left node (client)
    p.setBrush(main);
    p.drawEllipse(QPointF(3.2, 8.0), 1.7, 1.7);

    // right nodes (proxy endpoints)
    p.drawEllipse(QPointF(12.8, 3.6), 1.7, 1.7);
    p.drawEllipse(QPointF(12.8, 12.4), 1.7, 1.7);

    // routes
    p.drawLine(QPointF(4.9, 7.3), QPointF(11.1, 4.2));
    p.drawLine(QPointF(4.9, 8.7), QPointF(11.1, 11.8));

    // arrow head on the top route
    p.drawLine(QPointF(11.1, 4.2), QPointF(9.4, 4.1));
    p.drawLine(QPointF(11.1, 4.2), QPointF(10.5, 5.8));
    p.end();

    return QIcon(pm);
}

PS_ControlButton::PS_ControlButton(PS_Manager *manager, const std::function<void()> &onManage, QObject *parent)
    : AbstractButtonInterface(parent)
    , m_manager(manager)
    , m_onManage(onManage)
{
    connect(this, &AbstractButtonInterface::clicked, this, &PS_ControlButton::onClicked);
    connect(m_manager, &PS_Manager::activeProfileChanged, this, &PS_ControlButton::updateDisplay);
    connect(m_manager, &PS_Manager::profilesChanged, this, &PS_ControlButton::updateDisplay);

    updateDisplay();
}

QString PS_ControlButton::id() const
{
    return QStringLiteral("proxy-switcher");
}

QString PS_ControlButton::name() const
{
    return tr("Proxy Switcher");
}

void PS_ControlButton::onClicked(ClickController *controller)
{
    auto *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);

    buildProfileMenu(menu, m_manager, this,
                     [this](const QString &name) { m_manager->setActiveProfile(name); },
                     [this]() { m_onManage(); });

    connect(menu, &QMenu::aboutToHide, menu, [controller]() {
        controller->callPopupClosed();
    });

    menu->popup(controller->callPopupPosition(menu->sizeHint()));
    controller->popupOpened = true;
}

void PS_ControlButton::buildProfileMenu(QMenu *menu, PS_Manager *manager, QObject *context,
                                        const std::function<void(const QString &name)> &onPick,
                                        const std::function<void()> &onManage)
{
    menu->clear();

    const QString active = manager->activeProfile();

    QAction *systemAct = menu->addAction(QObject::tr("System proxy"));
    systemAct->setCheckable(true);
    systemAct->setChecked(active == PS_ACTIVE_SYSTEM);
    QObject::connect(systemAct, &QAction::triggered, context, [onPick]() { onPick(PS_ACTIVE_SYSTEM); });

    QAction *directAct = menu->addAction(QObject::tr("Direct connection"));
    directAct->setCheckable(true);
    directAct->setChecked(active == PS_ACTIVE_DIRECT);
    QObject::connect(directAct, &QAction::triggered, context, [onPick]() { onPick(PS_ACTIVE_DIRECT); });

    menu->addSeparator();

    const auto profiles = manager->profiles();
    for (const PS_Profile &profile : profiles) {
        QAction *act = menu->addAction(profile.name);
        act->setCheckable(true);
        act->setChecked(active == profile.name);
        act->setToolTip(profile.summary());
        const QString name = profile.name;
        QObject::connect(act, &QAction::triggered, context, [onPick, name]() { onPick(name); });
    }

    if (profiles.isEmpty()) {
        menu->addAction(QObject::tr("(no profiles defined)"))->setEnabled(false);
    }

    menu->addSeparator();

    QAction *offAct = menu->addAction(QObject::tr("Disable takeover (browser preferences)"));
    offAct->setCheckable(true);
    offAct->setChecked(active == PS_ACTIVE_OFF);
    QObject::connect(offAct, &QAction::triggered, context, [onPick]() { onPick(PS_ACTIVE_OFF); });

    QAction *manageAct = menu->addAction(QObject::tr("Manage profiles..."));
    QObject::connect(manageAct, &QAction::triggered, context, [onManage]() { onManage(); });
}

void PS_ControlButton::updateDisplay()
{
    const bool takeover = m_manager->activeProfile() != PS_ACTIVE_OFF;
    setIcon(makeProxyIcon(takeover));
    setToolTip(tr("Proxy Switcher\nCurrent: %1").arg(m_manager->activeDisplayName()));
}
