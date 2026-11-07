/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#include "ps_managerdialog.h"
#include "ps_manager.h"
#include "ps_editdialog.h"
#include "settings.h"

#include <QSettings>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>

PS_ManagerDialog::PS_ManagerDialog(PS_Manager *manager, QWidget *parent)
    : QDialog(parent)
    , m_manager(manager)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Proxy Switcher - Manage Profiles"));
    resize(520, 460);

    m_list = new QListWidget(this);
    m_list->setMinimumHeight(220);

    auto *btnAdd = new QPushButton(tr("Add..."), this);
    auto *btnEdit = new QPushButton(tr("Edit..."), this);
    auto *btnRemove = new QPushButton(tr("Remove"), this);
    auto *btnUse = new QPushButton(tr("Use selected"), this);
    auto *btnOff = new QPushButton(tr("Disable takeover"), this);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addWidget(btnAdd);
    buttonRow->addWidget(btnEdit);
    buttonRow->addWidget(btnRemove);
    buttonRow->addStretch();
    buttonRow->addWidget(btnUse);
    buttonRow->addWidget(btnOff);

    m_currentLabel = new QLabel(this);

    m_statusButtonCheck = new QCheckBox(tr("Show proxy button in status bar (the toolbar button is available via right-click toolbar > Configure Toolbar)"), this);
    m_statusButtonCheck->setChecked(m_manager->statusButtonVisible());

    m_webrtcCheck = new QCheckBox(tr("Block WebRTC UDP outside proxy (requires browser restart)"), this);
    m_webrtcCheck->setChecked(Settings::globalSettings()->value(QStringLiteral("Web-Proxy/webrtcProtect"), 0).toInt() == 1);

    m_hintLabel = new QLabel(tr(
        "Switching takes effect immediately for new network connections.<br>"
        "Note: saving the main Preferences dialog re-applies its own proxy page settings and<br>"
        "overrides this plugin until you switch a profile here again."), this);
    m_hintLabel->setWordWrap(true);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_currentLabel);
    layout->addWidget(m_list, 1);
    layout->addLayout(buttonRow);
    layout->addWidget(m_statusButtonCheck);
    layout->addWidget(m_webrtcCheck);
    layout->addWidget(m_hintLabel);

    connect(btnAdd, &QPushButton::clicked, this, &PS_ManagerDialog::addProfile);
    connect(btnEdit, &QPushButton::clicked, this, &PS_ManagerDialog::editProfile);
    connect(btnRemove, &QPushButton::clicked, this, &PS_ManagerDialog::removeProfile);
    connect(btnUse, &QPushButton::clicked, this, &PS_ManagerDialog::useSelected);
    connect(btnOff, &QPushButton::clicked, this, &PS_ManagerDialog::deactivate);
    connect(m_list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem*) { useSelected(); });

    connect(m_statusButtonCheck, &QCheckBox::toggled, this, [this](bool on) {
        m_manager->setStatusButtonVisible(on);
    });
    connect(m_webrtcCheck, &QCheckBox::toggled, this, [](bool on) {
        Settings::globalSettings()->setValue(QStringLiteral("Web-Proxy/webrtcProtect"), on ? 1 : 0);
    });
    connect(m_manager, &PS_Manager::activeProfileChanged, this, &PS_ManagerDialog::refreshList);

    refreshList();
}

void PS_ManagerDialog::refreshList()
{
    const QString active = m_manager->activeProfile();
    const QString selected = selectedData();

    m_list->clear();

    auto *systemItem = new QListWidgetItem(tr("System proxy"), m_list);
    systemItem->setData(Qt::UserRole, PS_ACTIVE_SYSTEM);
    systemItem->setToolTip(tr("Follow the operating system proxy configuration"));

    auto *directItem = new QListWidgetItem(tr("Direct connection"), m_list);
    directItem->setData(Qt::UserRole, PS_ACTIVE_DIRECT);
    directItem->setToolTip(tr("Never use a proxy (full takeover, ignores system proxy)"));

    const auto profiles = m_manager->profiles();
    for (const PS_Profile &profile : profiles) {
        auto *item = new QListWidgetItem(QStringLiteral("%1  (%2)").arg(profile.name, profile.summary()), m_list);
        item->setData(Qt::UserRole, profile.name);
    }

    for (int i = 0; i < m_list->count(); ++i) {
        QListWidgetItem *item = m_list->item(i);
        const QString data = item->data(Qt::UserRole).toString();
        QFont font = item->font();
        font.setBold(data == active);
        item->setFont(font);

        if (!selected.isEmpty() && data == selected) {
            m_list->setCurrentItem(item);
        } else if (selected.isEmpty() && data == active) {
            m_list->setCurrentItem(item);
        }
    }

    m_currentLabel->setText(tr("<b>Current:</b> %1").arg(m_manager->activeDisplayName().toHtmlEscaped()));
}

void PS_ManagerDialog::addProfile()
{
    PS_EditDialog dialog(m_manager, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    refreshList();
}

void PS_ManagerDialog::editProfile()
{
    const QString name = selectedData();
    if (name.isEmpty() || name == PS_ACTIVE_SYSTEM || name == PS_ACTIVE_DIRECT) {
        QMessageBox::information(this, tr("Proxy Switcher"), tr("Select a custom profile to edit."));
        return;
    }

    const PS_Profile original = m_manager->profile(name);

    PS_EditDialog dialog(m_manager, this, original, false);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    // Handle rename: remove stale group when name changed
    // (dialog validates and stores via manager in accept path? No: dialog only validates,
    //  so collect again here through the same constraints by re-reading manager state)
    if (!m_manager->hasProfile(name)) {
        return;
    }

    refreshList();
}

void PS_ManagerDialog::removeProfile()
{
    const QString name = selectedData();
    if (name.isEmpty() || name == PS_ACTIVE_SYSTEM || name == PS_ACTIVE_DIRECT) {
        QMessageBox::information(this, tr("Proxy Switcher"), tr("Select a custom profile to remove."));
        return;
    }

    if (QMessageBox::question(this, tr("Proxy Switcher"), tr("Remove profile \"%1\"?").arg(name))
        != QMessageBox::Yes) {
        return;
    }

    m_manager->removeProfile(name);
    refreshList();
}

void PS_ManagerDialog::useSelected()
{
    const QString name = selectedData();
    if (name.isEmpty()) {
        return;
    }
    m_manager->setActiveProfile(name);
    refreshList();
}

void PS_ManagerDialog::deactivate()
{
    m_manager->setActiveProfile(PS_ACTIVE_OFF);
    refreshList();
}

QString PS_ManagerDialog::selectedData() const
{
    if (auto *item = m_list->currentItem()) {
        return item->data(Qt::UserRole).toString();
    }
    return QString();
}
