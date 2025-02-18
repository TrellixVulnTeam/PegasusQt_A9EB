/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgenericunixservices_p.h"
#include <QtGui/private/qtguiglobal_p.h>

#include <QtCore/QStandardPaths>
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <stdlib.h>

QT_BEGIN_NAMESPACE

#if QT_CONFIG(multiprocess)

enum { debug = 0 };

static inline QByteArray detectDesktopEnvironment()
{
    const QByteArray xdgCurrentDesktop = qgetenv("XDG_CURRENT_DESKTOP");
    if (!xdgCurrentDesktop.isEmpty())
        return xdgCurrentDesktop.toUpper(); // KDE, GNOME, UNITY, LXDE, MATE, XFCE...

    // Classic fallbacks
    if (!qEnvironmentVariableIsEmpty("KDE_FULL_SESSION"))
        return QByteArrayLiteral("KDE");
    if (!qEnvironmentVariableIsEmpty("GNOME_DESKTOP_SESSION_ID"))
        return QByteArrayLiteral("GNOME");

    // Fallback to checking $DESKTOP_SESSION (unreliable)
    const QByteArray desktopSession = qgetenv("DESKTOP_SESSION");
    if (desktopSession == "gnome")
        return QByteArrayLiteral("GNOME");
    if (desktopSession == "xfce")
        return QByteArrayLiteral("XFCE");

    return QByteArrayLiteral("UNKNOWN");
}

static inline bool checkExecutable(const QString &candidate, QString *result)
{
    *result = QStandardPaths::findExecutable(candidate);
    return !result->isEmpty();
}

static inline bool detectWebBrowser(const QByteArray &desktop,
                                    bool checkBrowserVariable,
                                    QString *browser)
{
    const char *browsers[] = {"google-chrome", "firefox", "mozilla", "opera"};

    browser->clear();
    if (checkExecutable(QStringLiteral("xdg-open"), browser))
        return true;

    if (checkBrowserVariable) {
        QByteArray browserVariable = qgetenv("DEFAULT_BROWSER");
        if (browserVariable.isEmpty())
            browserVariable = qgetenv("BROWSER");
        if (!browserVariable.isEmpty() && checkExecutable(QString::fromLocal8Bit(browserVariable), browser))
            return true;
    }

    if (desktop == QByteArray("KDE")) {
        // Konqueror launcher
        if (checkExecutable(QStringLiteral("kfmclient"), browser)) {
            browser->append(QLatin1String(" exec"));
            return true;
        }
    } else if (desktop == QByteArray("GNOME")) {
        if (checkExecutable(QStringLiteral("gnome-open"), browser))
            return true;
    }

    for (size_t i = 0; i < sizeof(browsers)/sizeof(char *); ++i)
        if (checkExecutable(QLatin1String(browsers[i]), browser))
            return true;
    return false;
}

static inline bool launch(const QString &launcher, const QUrl &url)
{
    const QString command = launcher + QLatin1Char(' ') + QLatin1String(url.toEncoded());
    if (debug)
        qDebug("Launching %s", qPrintable(command));
#if defined(QT_NO_PROCESS)
    const bool ok = ::system(qPrintable(command + QLatin1String(" &")));
#else
    const bool ok = QProcess::startDetached(command);
#endif
    if (!ok)
        qWarning("Launch failed (%s)", qPrintable(command));
    return ok;
}

QByteArray QGenericUnixServices::desktopEnvironment() const
{
    static const QByteArray result = detectDesktopEnvironment();
    return result;
}

bool QGenericUnixServices::openUrl(const QUrl &url)
{
    if (url.scheme() == QLatin1String("mailto"))
        return openDocument(url);

    if (m_webBrowser.isEmpty() && !detectWebBrowser(desktopEnvironment(), true, &m_webBrowser)) {
        qWarning("Unable to detect a web browser to launch '%s'", qPrintable(url.toString()));
        return false;
    }
    return launch(m_webBrowser, url);
}

bool QGenericUnixServices::openDocument(const QUrl &url)
{
    if (m_documentLauncher.isEmpty() && !detectWebBrowser(desktopEnvironment(), false, &m_documentLauncher)) {
        qWarning("Unable to detect a launcher for '%s'", qPrintable(url.toString()));
        return false;
    }
    return launch(m_documentLauncher, url);
}

#else
QByteArray QGenericUnixServices::desktopEnvironment() const
{
    return QByteArrayLiteral("UNKNOWN");
}

bool QGenericUnixServices::openUrl(const QUrl &url)
{
    qWarning("openUrl() not supported on this platform");
    return false;
}

bool QGenericUnixServices::openDocument(const QUrl &url)
{
    qWarning("openDocument() not supported on this platform");
    return false;
}

#endif // QT_NO_MULTIPROCESS

QT_END_NAMESPACE
