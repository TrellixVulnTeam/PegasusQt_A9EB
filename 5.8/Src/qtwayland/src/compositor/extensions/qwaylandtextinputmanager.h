/****************************************************************************
**
** Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDTEXTINPUTMANAGER_H
#define QWAYLANDTEXTINPUTMANAGER_H

#include <QtWaylandCompositor/QWaylandCompositorExtension>

#include <QtCore/QSize>

QT_BEGIN_NAMESPACE

class QWaylandTextInputManagerPrivate;

class Q_WAYLAND_COMPOSITOR_EXPORT QWaylandTextInputManager : public QWaylandCompositorExtensionTemplate<QWaylandTextInputManager>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWaylandTextInputManager)
public:
    QWaylandTextInputManager();
    QWaylandTextInputManager(QWaylandCompositor *compositor);

    void initialize() Q_DECL_OVERRIDE;

    static const struct wl_interface *interface();
    static QByteArray interfaceName();
};

QT_END_NAMESPACE

#endif // QWAYLANDTEXTINPUTMANAGER_H
