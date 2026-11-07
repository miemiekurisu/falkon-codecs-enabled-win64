/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#ifndef PS_MANAGERDIALOG_H
#define PS_MANAGERDIALOG_H

#include <QDialog>

class PS_Manager;
class QListWidget;
class QLabel;
class QCheckBox;

class PS_ManagerDialog : public QDialog
{
    Q_OBJECT
public:
    PS_ManagerDialog(PS_Manager *manager, QWidget *parent = nullptr);

private Q_SLOTS:
    void refreshList();
    void addProfile();
    void editProfile();
    void removeProfile();
    void useSelected();
    void deactivate();

private:
    QString selectedData() const;

    PS_Manager *m_manager;
    QListWidget *m_list;
    QLabel *m_currentLabel;
    QLabel *m_hintLabel;
    QCheckBox *m_statusButtonCheck;
    QCheckBox *m_webrtcCheck;
};

#endif // PS_MANAGERDIALOG_H
