/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the config.tests of the Qt Toolkit.
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

#ifndef QWAYLANDSHELLSURFACE_H
#define QWAYLANDSHELLSURFACE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QSize>
#include <QObject>

#include <wayland-client.h>

#include <QtWaylandClient/private/qwayland-wayland.h>
#include <QtWaylandClient/qtwaylandclientglobal.h>

QT_BEGIN_NAMESPACE

class QVariant;
class QWindow;

namespace QtWaylandClient {

class QWaylandWindow;
class QWaylandInputDevice;

class Q_WAYLAND_CLIENT_EXPORT QWaylandShellSurface : public QObject
{
    Q_OBJECT
public:
    explicit QWaylandShellSurface(QWaylandWindow *window);
    virtual ~QWaylandShellSurface() {}
    virtual void resize(QWaylandInputDevice * /*inputDevice*/, enum wl_shell_surface_resize /*edges*/)
    {}

    virtual void move(QWaylandInputDevice * /*inputDevice*/) {}
    virtual void setTitle(const QString & /*title*/) {}
    virtual void setAppId(const QString & /*appId*/) {}

    virtual void setWindowFlags(Qt::WindowFlags flags);

    virtual bool isExposed() const { return true; }

    virtual void raise() {}
    virtual void lower() {}
    virtual void setContentOrientationMask(Qt::ScreenOrientations orientation) { Q_UNUSED(orientation) }

    virtual void sendProperty(const QString &name, const QVariant &value);

    inline QWaylandWindow *window() { return m_window; }

    virtual void setType(Qt::WindowType type, QWaylandWindow *transientParent) = 0;

protected:
    virtual void setMaximized() {}
    virtual void setFullscreen() {}
    virtual void setNormal() {}
    virtual void setMinimized() {}

private:
    QWaylandWindow *m_window;
    friend class QWaylandWindow;
};

}

QT_END_NAMESPACE

#endif // QWAYLANDSHELLSURFACE_H
