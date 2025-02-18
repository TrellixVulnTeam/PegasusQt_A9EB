/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
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

#include "qquickmenuseparator_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype MenuSeparator
    \inherits Control
    \instantiates QQuickMenuSeparator
    \inqmlmodule QtQuick.Controls
    \since 5.8
    \ingroup qtquickcontrols2-separators
    \brief Separates a group of items in a menu from adjacent items.

    MenuSeparator is used to visually distinguish between groups of items in a
    menu by separating them with a line.

    \image qtquickcontrols2-menuseparator.png

    \quotefromfile qtquickcontrols2-menuseparator-custom.qml
    \skipto import QtQuick 2.6
    \printuntil import QtQuick.Controls 2.1
    \skipto Menu
    \printto contentItem.parent: window
    \skipline contentItem.parent: window
    \printuntil text: qsTr("Exit")
    \printuntil }
    \printuntil }

    \sa {Customizing MenuSeparator}, {Separator Controls}
*/

QQuickMenuSeparator::QQuickMenuSeparator(QQuickItem *parent) :
    QQuickControl(parent)
{
}

#ifndef QT_NO_ACCESSIBILITY
QAccessible::Role QQuickMenuSeparator::accessibleRole() const
{
    return QAccessible::Separator;
}
#endif

QT_END_NAMESPACE
