/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#ifndef PS_EDITDIALOG_H
#define PS_EDITDIALOG_H

#include "ps_profile.h"

#include <QDialog>

class PS_Manager;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QLabel;
class QPlainTextEdit;

class PS_EditDialog : public QDialog
{
    Q_OBJECT
public:
    PS_EditDialog(PS_Manager *manager, QWidget *parent, const PS_Profile &profile = PS_Profile(), bool isNew = true);

    void accept() override;

private Q_SLOTS:
    void typeChanged(int index);
    void startTest();
    void onTestFinished(bool success, const QString &message);

private:
    bool validateAndCollect(PS_Profile &out);

    PS_Manager *m_manager;
    bool m_isNew;
    QString m_originalName;

    QLineEdit *m_nameEdit;
    QComboBox *m_typeCombo;
    QLineEdit *m_hostEdit;
    QSpinBox *m_portSpin;
    QLineEdit *m_userEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_testUrlEdit;
    QLabel *m_testResultLabel;
    QPlainTextEdit *m_bypassEdit;
};

#endif // PS_EDITDIALOG_H
