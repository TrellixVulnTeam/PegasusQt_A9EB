/****************************************************************************
**
** Copyright (C) 2013 Samuel Gaist <samuel.gaist@edeltech.ch>
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

#ifndef QWINDOWSINTEGRATION_H
#define QWINDOWSINTEGRATION_H

#include <qpa/qplatformintegration.h>
#include <QtCore/QScopedPointer>
#include <QtFontDatabaseSupport/private/qwindowsfontdatabase_p.h>

QT_BEGIN_NAMESPACE

struct QWindowsIntegrationPrivate;
struct QWindowsWindowData;
class QWindowsWindow;
class QWindowsStaticOpenGLContext;

class QWindowsIntegration : public QPlatformIntegration
{
public:
    enum Options { // Options to be passed on command line.
        FontDatabaseFreeType = 0x1,
        FontDatabaseNative = 0x2,
        DisableArb = 0x4,
        NoNativeDialogs = 0x8,
        XpNativeDialogs = 0x10,
        DontPassOsMouseEventsSynthesizedFromTouch = 0x20, // Do not pass OS-generated mouse events from touch.
        // Keep in sync with QWindowsFontDatabase::FontOptions
        DontUseDirectWriteFonts = QWindowsFontDatabase::DontUseDirectWriteFonts,
        DontUseColorFonts = QWindowsFontDatabase::DontUseColorFonts
    };

    explicit QWindowsIntegration(const QStringList &paramList);
    virtual ~QWindowsIntegration();

    bool hasCapability(QPlatformIntegration::Capability cap) const Q_DECL_OVERRIDE;

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
#ifndef QT_NO_OPENGL
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const Q_DECL_OVERRIDE;
    QOpenGLContext::OpenGLModuleType openGLModuleType() Q_DECL_OVERRIDE;
    static QWindowsStaticOpenGLContext *staticOpenGLContext();
#endif
    QAbstractEventDispatcher *createEventDispatcher() const Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;
#ifndef QT_NO_CLIPBOARD
    QPlatformClipboard *clipboard() const Q_DECL_OVERRIDE;
#  ifndef QT_NO_DRAGANDDROP
    QPlatformDrag *drag() const Q_DECL_OVERRIDE;
#  endif
#endif // !QT_NO_CLIPBOARD
    QPlatformInputContext *inputContext() const Q_DECL_OVERRIDE;
#ifndef QT_NO_ACCESSIBILITY
    QPlatformAccessibility *accessibility() const Q_DECL_OVERRIDE;
#endif
    QPlatformFontDatabase *fontDatabase() const Q_DECL_OVERRIDE;
    QStringList themeNames() const Q_DECL_OVERRIDE;
    QPlatformTheme *createPlatformTheme(const QString &name) const Q_DECL_OVERRIDE;
    QPlatformServices *services() const Q_DECL_OVERRIDE;
    QVariant styleHint(StyleHint hint) const Q_DECL_OVERRIDE;

    Qt::KeyboardModifiers queryKeyboardModifiers() const Q_DECL_OVERRIDE;
    QList<int> possibleKeys(const QKeyEvent *e) const Q_DECL_OVERRIDE;

    static QWindowsIntegration *instance() { return m_instance; }

    inline void emitScreenAdded(QPlatformScreen *s, bool isPrimary = false) { screenAdded(s, isPrimary); }
    inline void emitDestroyScreen(QPlatformScreen *s) { destroyScreen(s); }

    unsigned options() const;

    void beep() const Q_DECL_OVERRIDE;

#if !defined(QT_NO_SESSIONMANAGER)
    QPlatformSessionManager *createPlatformSessionManager(const QString &id, const QString &key) const Q_DECL_OVERRIDE;
#endif

protected:
    virtual QWindowsWindow *createPlatformWindowHelper(QWindow *window, const QWindowsWindowData &) const;

private:
    QScopedPointer<QWindowsIntegrationPrivate> d;

    static QWindowsIntegration *m_instance;
};

QT_END_NAMESPACE

#endif
