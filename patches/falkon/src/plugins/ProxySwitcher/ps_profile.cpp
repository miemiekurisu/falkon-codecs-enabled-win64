/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#include "ps_profile.h"

#include <QSettings>

QString PS_Profile::typeDescription() const
{
    switch (type) {
    case HttpProxy:
        return QStringLiteral("HTTP");
    case Socks5Proxy:
        return QStringLiteral("SOCKS5");
    case Direct:
        return QStringLiteral("Direct");
    case System:
        return QStringLiteral("System");
    }
    return QStringLiteral("HTTP");
}

QString PS_Profile::summary() const
{
    if (type == HttpProxy || type == Socks5Proxy) {
        QString summary = QStringLiteral("%1 %2:%3").arg(typeDescription(), host, QString::number(port));
        if (!bypassRules.isEmpty()) {
            summary += QStringLiteral(" (+%1 bypass rules)").arg(bypassRules.count());
        }
        return summary;
    }
    return typeDescription();
}

QNetworkProxy PS_Profile::toNetworkProxy() const
{
    switch (type) {
    case Direct:
        return QNetworkProxy(QNetworkProxy::NoProxy);
    case System:
        return QNetworkProxy();
    case HttpProxy:
    case Socks5Proxy:
    default:
        QNetworkProxy proxy;
        proxy.setType(type == HttpProxy ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy);
        proxy.setHostName(host);
        proxy.setPort(quint16(port));
        proxy.setUser(user);
        proxy.setPassword(password);
        return proxy;
    }
}

void PS_Profile::loadFromSettings(QSettings &settings)
{
    type = Type(settings.value(QStringLiteral("Type"), int(HttpProxy)).toInt());
    host = settings.value(QStringLiteral("HostName"), QString()).toString();
    port = settings.value(QStringLiteral("Port"), 0).toInt();
    user = settings.value(QStringLiteral("Username"), QString()).toString();
    password = settings.value(QStringLiteral("Password"), QString()).toString();
    bypassRules = settings.value(QStringLiteral("BypassRules"), QStringList()).toStringList();
}

void PS_Profile::saveToSettings(QSettings &settings) const
{
    settings.setValue(QStringLiteral("Type"), int(type));
    settings.setValue(QStringLiteral("HostName"), host);
    settings.setValue(QStringLiteral("Port"), port);
    settings.setValue(QStringLiteral("Username"), user);
    settings.setValue(QStringLiteral("Password"), password);
    settings.setValue(QStringLiteral("BypassRules"), bypassRules);
}
