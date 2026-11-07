/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#include "ps_manager.h"
#include "settings.h"
#include "mainapplication.h"
#include "networkmanager.h"

#include <QSettings>
#include <QSet>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxyFactory>
#include <QPointer>
#include <QTimer>

#include <algorithm>
#include <memory>

static const int TestTimeoutMs = 10000;

PS_Manager::PS_Manager(const QString &settingsPath, QObject *parent)
    : QObject(parent)
    , m_settingsFile(settingsPath + QStringLiteral("/proxyswitcher.ini"))
    , m_testNam(nullptr)
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);

    // NOTE: do not use beginGroup()+childGroups() here: inside the Falkon
    // process QSettings::childGroups() reports nested INI sections
    // inconsistently; parse the key paths explicitly instead.
    QSet<QString> profileNames;
    const QStringList allKeys = settings.allKeys();
    for (const QString &key : allKeys) {
        if (!key.startsWith(QStringLiteral("Profiles/"))) {
            continue;
        }
        const QString rest = key.mid(QStringLiteral("Profiles/").size());
        const int slash = rest.indexOf(QLatin1Char('/'));
        if (slash > 0) {
            profileNames.insert(rest.left(slash));
        }
    }

    const QStringList names = profileNames.values();
    for (const QString &name : names) {
        if (name.isEmpty()) {
            continue;
        }

        PS_Profile profile;
        profile.name = name;

        settings.beginGroup(QStringLiteral("Profiles"));
        settings.beginGroup(name);
        profile.loadFromSettings(settings);
        settings.endGroup();
        settings.endGroup();

        m_profiles.append(profile);
    }

    m_activeProfile = settings.value(QStringLiteral("ActiveProfile"), PS_ACTIVE_OFF).toString();
}

QList<PS_Profile> PS_Manager::profiles() const
{
    return m_profiles;
}

bool PS_Manager::hasProfile(const QString &name) const
{
    return std::any_of(m_profiles.cbegin(), m_profiles.cend(),
                       [&name](const PS_Profile &p) { return p.name == name; });
}

PS_Profile PS_Manager::profile(const QString &name) const
{
    for (const PS_Profile &profile : m_profiles) {
        if (profile.name == name) {
            return profile;
        }
    }
    return PS_Profile();
}

void PS_Manager::saveProfile(const PS_Profile &profile)
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Profiles"));
    settings.beginGroup(profile.name);
    profile.saveToSettings(settings);
    settings.endGroup();
    settings.endGroup();

    bool found = false;
    for (PS_Profile &stored : m_profiles) {
        if (stored.name == profile.name) {
            stored = profile;
            found = true;
            break;
        }
    }
    if (!found) {
        m_profiles.append(profile);
    }

    Q_EMIT profilesChanged();

    if (m_activeProfile == profile.name) {
        applyActiveProfile();
    }
}

void PS_Manager::removeProfile(const QString &name)
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Profiles"));
    settings.beginGroup(name);
    settings.remove(QString());
    settings.endGroup();
    settings.endGroup();

    m_profiles.removeIf([&name](const PS_Profile &p) { return p.name == name; });

    if (m_activeProfile == name) {
        setActiveProfile(PS_ACTIVE_OFF);
    }

    Q_EMIT profilesChanged();
}

QString PS_Manager::activeProfile() const
{
    return m_activeProfile;
}

void PS_Manager::setActiveProfile(const QString &name)
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.setValue(QStringLiteral("ActiveProfile"), name);

    m_activeProfile = name;
    applyActiveProfile();
}

QString PS_Manager::activeDisplayName() const
{
    if (m_activeProfile == PS_ACTIVE_DIRECT) {
        return tr("Direct connection");
    }
    if (m_activeProfile == PS_ACTIVE_SYSTEM) {
        return tr("System proxy");
    }
    if (m_activeProfile == PS_ACTIVE_OFF) {
        return tr("Disabled (browser preferences in control)");
    }
    if (hasProfile(m_activeProfile)) {
        return m_activeProfile;
    }
    return tr("Missing profile (disabled)");
}

void PS_Manager::applyActiveProfile()
{
    const bool brokenRef = !m_activeProfile.isEmpty()
                           && m_activeProfile != PS_ACTIVE_DIRECT
                           && m_activeProfile != PS_ACTIVE_SYSTEM
                           && !hasProfile(m_activeProfile);

    if (m_activeProfile == PS_ACTIVE_OFF || brokenRef) {
        // Release control: preferences page is in charge again
        qputenv("FALKON_PROXY_BYPASS", QByteArray());

        Settings settings;
        settings.setValue(QStringLiteral("Web-Proxy/Takeover"), false);
        settings.sync();

        if (mApp && mApp->networkManager()) {
            mApp->networkManager()->loadSettings();
        }

        Q_EMIT activeProfileChanged();
        return;
    }

    // Full takeover: preferences page must not touch the proxy anymore
    Settings settings;
    settings.setValue(QStringLiteral("Web-Proxy/Takeover"), true);
    settings.sync();

    if (m_activeProfile == PS_ACTIVE_DIRECT) {
        qputenv("FALKON_PROXY_BYPASS", QByteArray());
        QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::NoProxy));
        QNetworkProxyFactory::setUseSystemConfiguration(false);
    } else if (m_activeProfile == PS_ACTIVE_SYSTEM) {
        qputenv("FALKON_PROXY_BYPASS", QByteArray());
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    } else {
        const PS_Profile profile = this->profile(m_activeProfile);
        qputenv("FALKON_PROXY_BYPASS", profile.bypassRules.join(QLatin1Char(',')).toUtf8());
        QNetworkProxy::setApplicationProxy(profile.toNetworkProxy());
        QNetworkProxyFactory::setUseSystemConfiguration(false);
    }

    Q_EMIT activeProfileChanged();
}

bool PS_Manager::statusButtonVisible() const
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    return settings.value(QStringLiteral("ShowStatusButton"), true).toBool();
}

void PS_Manager::setStatusButtonVisible(bool visible)
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.setValue(QStringLiteral("ShowStatusButton"), visible);
}

QString PS_Manager::testUrl() const
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    return settings.value(QStringLiteral("TestUrl"),
                          QStringLiteral("https://www.google.com/generate_204")).toString();
}

void PS_Manager::setTestUrl(const QString &url)
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.setValue(QStringLiteral("TestUrl"), url);
}

void PS_Manager::testConfiguration(const PS_Profile &profile, const QString &url)
{
    if (!m_testNam) {
        m_testNam = new QNetworkAccessManager(this);
    }

    QNetworkProxy proxy;
    switch (profile.type) {
    case PS_Profile::System: {
        const auto proxies = QNetworkProxyFactory::systemProxyForQuery(QNetworkProxyQuery(QUrl(url)));
        proxy = proxies.isEmpty() ? QNetworkProxy(QNetworkProxy::NoProxy) : proxies.first();
        break;
    }
    default:
        proxy = profile.toNetworkProxy();
        break;
    }

    m_testNam->setProxy(proxy);

    QNetworkRequest request{QUrl(url)};
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0 (ProxySwitcher connectivity test)"));

    QNetworkReply *reply = m_testNam->get(request);
    QPointer<PS_Manager> guard(this);
    auto timedOut = std::make_shared<bool>(false);

    auto *timeout = new QTimer(reply);
    timeout->setSingleShot(true);
    QObject::connect(timeout, &QTimer::timeout, reply, [reply, timedOut]() {
        *timedOut = true;
        reply->abort();
    });
    timeout->start(TestTimeoutMs);

    QObject::connect(reply, &QNetworkReply::finished, reply, [guard, reply, timedOut]() {
        if (*timedOut) {
            if (guard) {
                Q_EMIT guard->testFinished(false, QObject::tr("Test timed out (proxy did not respond within 10 seconds)."));
            }
        } else if (guard) {
            guard->replyTestResult(reply);
        }
        reply->deleteLater();
    });
}

void PS_Manager::replyTestResult(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        Q_EMIT testFinished(false, reply->errorString());
        return;
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode > 0) {
        Q_EMIT testFinished(true, tr("Success - HTTP %1 received through proxy.").arg(statusCode));
    } else {
        Q_EMIT testFinished(true, tr("Success - connection established."));
    }
}
