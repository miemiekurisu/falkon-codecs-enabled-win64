/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#include "ps_editdialog.h"
#include "ps_manager.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QPushButton>
#include <QMessageBox>

PS_EditDialog::PS_EditDialog(PS_Manager *manager, QWidget *parent, const PS_Profile &profile, bool isNew)
    : QDialog(parent)
    , m_manager(manager)
    , m_isNew(isNew)
    , m_originalName(profile.name)
{
    setWindowTitle(isNew ? tr("New Proxy Profile") : tr("Edit Proxy Profile"));
    setMinimumWidth(420);

    m_nameEdit = new QLineEdit(profile.name, this);
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem(tr("HTTP proxy"), int(PS_Profile::HttpProxy));
    m_typeCombo->addItem(tr("SOCKS5 proxy"), int(PS_Profile::Socks5Proxy));
    m_typeCombo->addItem(tr("Direct connection"), int(PS_Profile::Direct));
    m_typeCombo->addItem(tr("System proxy"), int(PS_Profile::System));

    const int typeIndex = m_typeCombo->findData(int(profile.type));
    m_typeCombo->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);

    m_hostEdit = new QLineEdit(profile.host, this);
    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(0, 65535);
    m_portSpin->setValue(profile.port > 0 ? profile.port : 8080);
    m_userEdit = new QLineEdit(profile.user, this);
    m_passwordEdit = new QLineEdit(profile.password, this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_testUrlEdit = new QLineEdit(manager->testUrl(), this);
    m_testResultLabel = new QLabel(this);
    m_testResultLabel->setWordWrap(true);

    m_bypassEdit = new QPlainTextEdit(this);
    m_bypassEdit->setPlainText(profile.bypassRules.join(QLatin1Char('\n')));
    m_bypassEdit->setMaximumHeight(80);
    m_bypassEdit->setPlaceholderText(QStringLiteral("*.example.com\n192.168.*\n10.0.0.0/8"));

    auto *bypassHint = new QLabel(tr("Bypass list - one rule per line, these addresses connect <b>directly</b> (wildcards, IPs, CIDR ranges and *.domains supported):"), this);
    bypassHint->setWordWrap(true);

    auto *form = new QFormLayout;
    form->addRow(tr("Profile name:"), m_nameEdit);
    form->addRow(tr("Proxy type:"), m_typeCombo);
    form->addRow(tr("Host:"), m_hostEdit);
    form->addRow(tr("Port:"), m_portSpin);
    form->addRow(tr("Username:"), m_userEdit);
    form->addRow(tr("Password:"), m_passwordEdit);
    form->addRow(bypassHint);
    form->addRow(tr("Bypass list:"), m_bypassEdit);
    form->addRow(tr("Test URL:"), m_testUrlEdit);

    auto *testButton = new QPushButton(tr("Test"), this);
    auto *testRow = new QHBoxLayout;
    testRow->addWidget(testButton);
    testRow->addWidget(m_testResultLabel, 1);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addLayout(testRow);
    layout->addWidget(buttons);

    connect(m_typeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PS_EditDialog::typeChanged);
    connect(testButton, &QPushButton::clicked, this, &PS_EditDialog::startTest);
    connect(manager, &PS_Manager::testFinished, this, &PS_EditDialog::onTestFinished);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    typeChanged(m_typeCombo->currentIndex());
}

void PS_EditDialog::accept()
{
    PS_Profile profile;
    if (!validateAndCollect(profile)) {
        return;
    }

    m_manager->saveProfile(profile);

    if (!m_isNew && profile.name != m_originalName) {
        if (m_manager->activeProfile() == m_originalName) {
            m_manager->setActiveProfile(profile.name);
        }
        m_manager->removeProfile(m_originalName);
    }

    QDialog::accept();
}

void PS_EditDialog::typeChanged(int index)
{
    Q_UNUSED(index)
    const bool manual = m_typeCombo->currentData().toInt() == int(PS_Profile::HttpProxy)
                        || m_typeCombo->currentData().toInt() == int(PS_Profile::Socks5Proxy);

    m_hostEdit->setEnabled(manual);
    m_portSpin->setEnabled(manual);
    m_userEdit->setEnabled(manual);
    m_passwordEdit->setEnabled(manual);
    m_bypassEdit->setEnabled(manual);
}

void PS_EditDialog::startTest()
{
    PS_Profile profile;
    if (!validateAndCollect(profile)) {
        return;
    }

    const QString url = m_testUrlEdit->text().trimmed();
    if (url.isEmpty()) {
        return;
    }

    m_manager->setTestUrl(url);
    m_testResultLabel->setText(tr("Testing..."));
    m_manager->testConfiguration(profile, url);
}

void PS_EditDialog::onTestFinished(bool success, const QString &message)
{
    m_testResultLabel->setText(message);
    m_testResultLabel->setStyleSheet(success ? QStringLiteral("color: green;") : QStringLiteral("color: red;"));
}

bool PS_EditDialog::validateAndCollect(PS_Profile &out)
{
    out.name = m_nameEdit->text().trimmed();
    out.type = PS_Profile::Type(m_typeCombo->currentData().toInt());
    out.host = m_hostEdit->text().trimmed();
    out.port = m_portSpin->value();
    out.user = m_userEdit->text();
    out.password = m_passwordEdit->text();
    const QStringList rawRules = m_bypassEdit->toPlainText()
                                     .split(QRegularExpression(QStringLiteral("[\\n,;]")), Qt::SkipEmptyParts);
    for (const QString &rawRule : rawRules) {
        const QString rule = rawRule.trimmed();
        if (!rule.isEmpty()) {
            out.bypassRules.append(rule);
        }
    }

    if (out.name.isEmpty()) {
        QMessageBox::warning(this, tr("Proxy Switcher"), tr("Profile name cannot be empty."));
        return false;
    }

    // Custom profile names must not shadow the built-in pseudo profiles
    if (out.name == PS_ACTIVE_DIRECT || out.name == PS_ACTIVE_SYSTEM) {
        QMessageBox::warning(this, tr("Proxy Switcher"), tr("This name is reserved."));
        return false;
    }

    if (out.type == PS_Profile::HttpProxy || out.type == PS_Profile::Socks5Proxy) {
        if (out.host.isEmpty()) {
            QMessageBox::warning(this, tr("Proxy Switcher"), tr("Proxy host cannot be empty."));
            return false;
        }
        if (out.port < 1 || out.port > 65535) {
            QMessageBox::warning(this, tr("Proxy Switcher"), tr("Proxy port must be between 1 and 65535."));
            return false;
        }
    }

    if (m_isNew && m_manager->hasProfile(out.name)) {
        QMessageBox::warning(this, tr("Proxy Switcher"), tr("A profile with this name already exists."));
        return false;
    }
    if (!m_isNew && out.name != m_originalName && m_manager->hasProfile(out.name)) {
        QMessageBox::warning(this, tr("Proxy Switcher"), tr("A profile with this name already exists."));
        return false;
    }

    return true;
}
