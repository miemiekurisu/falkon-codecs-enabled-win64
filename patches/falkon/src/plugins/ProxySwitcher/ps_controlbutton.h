/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#ifndef PS_CONTROLBUTTON_H
#define PS_CONTROLBUTTON_H

#include "abstractbuttoninterface.h"

#include <functional>

class PS_Manager;
class QMenu;

class PS_ControlButton : public AbstractButtonInterface
{
    Q_OBJECT
public:
    explicit PS_ControlButton(PS_Manager *manager, const std::function<void()> &onManage, QObject *parent = nullptr);

    QString id() const override;
    QString name() const override;

    // Shared menu builder used by the button popup and Extensions menu
    static void buildProfileMenu(QMenu *menu, PS_Manager *manager, QObject *context,
                                 const std::function<void(const QString &name)> &onPick,
                                 const std::function<void()> &onManage);

private Q_SLOTS:
    void onClicked(ClickController *controller);
    void updateDisplay();

private:
    PS_Manager *m_manager;
    std::function<void()> m_onManage;
};

#endif // PS_CONTROLBUTTON_H
