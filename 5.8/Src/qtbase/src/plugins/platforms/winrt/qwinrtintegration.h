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

#ifndef QWINRTINTEGRATION_H
#define QWINRTINTEGRATION_H

#include <qpa/qplatformintegration.h>

namespace ABI {
    namespace Windows {
        namespace ApplicationModel {
            struct ISuspendingEventArgs;
        }
        namespace Foundation {
            struct IAsyncAction;
        }
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
        namespace Phone {
            namespace UI {
                namespace Input {
                    struct IBackPressedEventArgs;
                    struct ICameraEventArgs;
                }
            }
        }
#endif
    }
}
struct IAsyncInfo;
struct IInspectable;

QT_BEGIN_NAMESPACE

class QAbstractEventDispatcher;

class QWinRTIntegrationPrivate;
class QWinRTIntegration : public QPlatformIntegration
{
private:
    explicit QWinRTIntegration();
public:
    ~QWinRTIntegration();

    static QWinRTIntegration *create()
    {
        QScopedPointer<QWinRTIntegration> integration(new QWinRTIntegration);
        return integration->succeeded() ? integration.take() : nullptr;
    }

    bool succeeded() const;

    bool hasCapability(QPlatformIntegration::Capability cap) const Q_DECL_OVERRIDE;
    QVariant styleHint(StyleHint hint) const Q_DECL_OVERRIDE;

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const Q_DECL_OVERRIDE;
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const Q_DECL_OVERRIDE;
    QAbstractEventDispatcher *createEventDispatcher() const Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;
    QPlatformFontDatabase *fontDatabase() const Q_DECL_OVERRIDE;
    QPlatformInputContext *inputContext() const Q_DECL_OVERRIDE;
    QPlatformServices *services() const Q_DECL_OVERRIDE;
    QPlatformClipboard *clipboard() const Q_DECL_OVERRIDE;
#ifndef QT_NO_DRAGANDDROP
    QPlatformDrag *drag() const Q_DECL_OVERRIDE;
#endif

    Qt::KeyboardModifiers queryKeyboardModifiers() const Q_DECL_OVERRIDE;

    QStringList themeNames() const Q_DECL_OVERRIDE;
    QPlatformTheme *createPlatformTheme(const QString &name) const Q_DECL_OVERRIDE;

    QPlatformOffscreenSurface *createPlatformOffscreenSurface(QOffscreenSurface *surface) const Q_DECL_OVERRIDE;
private:
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
    HRESULT onBackButtonPressed(IInspectable *, ABI::Windows::Phone::UI::Input::IBackPressedEventArgs *args);
    HRESULT onCameraPressed(IInspectable *, ABI::Windows::Phone::UI::Input::ICameraEventArgs *);
    HRESULT onCameraHalfPressed(IInspectable *, ABI::Windows::Phone::UI::Input::ICameraEventArgs *);
    HRESULT onCameraReleased(IInspectable *, ABI::Windows::Phone::UI::Input::ICameraEventArgs *);
#endif
    HRESULT onSuspended(IInspectable *, ABI::Windows::ApplicationModel::ISuspendingEventArgs *);
    HRESULT onResume(IInspectable *, IInspectable *);

    QScopedPointer<QWinRTIntegrationPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QWinRTIntegration)
};

QT_END_NAMESPACE

#endif // QWINRTINTEGRATION_H
