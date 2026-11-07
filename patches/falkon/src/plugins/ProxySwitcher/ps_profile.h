/* ============================================================
* ProxySwitcher - SwitchyOmega-like proxy profiles for Falkon
* Unofficial third-party plugin (falkon-codecs-enabled-win64)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* ============================================================ */
#ifndef PS_PROFILE_H
#define PS_PROFILE_H

#include <QString>
#include <QStringList>
#include <QNetworkProxy>

class QSettings;

class PS_Profile
{
public:
    enum Type {
        HttpProxy = 0,
        Socks5Proxy = 1,
        Direct = 2,
        System = 3,
    };

    QString name;
    Type type = HttpProxy;
    QString host;
    int port = 0;
    QString user;
    QString password;
    // Chromium bypass rules for direct connection (*.example.com, 192.168.*, 10.0.0.0/8, ...)
    QStringList bypassRules;

    QString typeDescription() const;
    QString summary() const;

    QNetworkProxy toNetworkProxy() const;

    void loadFromSettings(QSettings &settings);
    void saveToSettings(QSettings &settings) const;
};

#endif // PS_PROFILE_H
