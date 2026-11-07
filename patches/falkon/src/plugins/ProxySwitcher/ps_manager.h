/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#ifndef PS_MANAGER_H
#define PS_MANAGER_H

#include "ps_profile.h"

#include <QObject>
#include <QList>

class QNetworkAccessManager;
class QNetworkReply;

// Pseudo-profile names (SwitchyOmega-like built-ins)
#define PS_ACTIVE_OFF QStringLiteral("")
#define PS_ACTIVE_DIRECT QStringLiteral("__direct__")
#define PS_ACTIVE_SYSTEM QStringLiteral("__system__")

class PS_Manager : public QObject
{
    Q_OBJECT
public:
    explicit PS_Manager(const QString &settingsPath, QObject *parent = nullptr);

    QList<PS_Profile> profiles() const;
    bool hasProfile(const QString &name) const;
    PS_Profile profile(const QString &name) const;
    void saveProfile(const PS_Profile &profile);
    void removeProfile(const QString &name);

    // Active selection: PS_ACTIVE_OFF / PS_ACTIVE_DIRECT / PS_ACTIVE_SYSTEM / profile name
    QString activeProfile() const;
    void setActiveProfile(const QString &name);
    QString activeDisplayName() const;

    // Force re-application of the stored active profile (after startup)
    void applyActiveProfile();

    bool statusButtonVisible() const;
    void setStatusButtonVisible(bool visible);

    QString testUrl() const;
    void setTestUrl(const QString &url);

    void testConfiguration(const PS_Profile &profile, const QString &url);

Q_SIGNALS:
    void activeProfileChanged();
    void profilesChanged();
    void testFinished(bool success, const QString &message);

private:
    void replyTestResult(QNetworkReply *reply);

    QString m_settingsFile;
    QList<PS_Profile> m_profiles;
    QString m_activeProfile;
    QNetworkAccessManager *m_testNam;
};

#endif // PS_MANAGER_H
