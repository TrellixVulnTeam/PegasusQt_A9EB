/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
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

#include "qquickuniversalfocusrectangle_p.h"

#include <QtGui/qpixmap.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpixmapcache.h>
#include <QtQuick/private/qquickitem_p.h>

QT_BEGIN_NAMESPACE

QQuickUniversalFocusRectangle::QQuickUniversalFocusRectangle(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    QQuickItemPrivate::get(this)->setTransparentForPositioner(true);
}

void QQuickUniversalFocusRectangle::paint(QPainter *painter)
{
    if (!isVisible() || width() <= 0 || height() <= 0)
        return;

    QRect bounds = boundingRect().toAlignedRect();
    const int boundsWidth = bounds.width();
    const int boundsHeight = bounds.width();
    const QString key = QStringLiteral("qquickuniversalfocusrectangle_%1_%2").arg(QString::number(boundsWidth), QString::number(boundsHeight));

    QPixmap pixmap(boundsWidth, boundsHeight);
    if (!QPixmapCache::find(key, &pixmap)) {
        bounds.adjust(0, 0, -1, -1);
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);

        QPen pen;
        pen.setWidth(1);
        pen.setColor(Qt::white);
        p.setPen(pen);
        p.drawRect(bounds);

        pen.setColor(Qt::black);
        pen.setDashPattern(QVector<qreal>(2, 1));
        p.setPen(pen);
        p.drawRect(bounds);

        QPixmapCache::insert(key, pixmap);
    }
    painter->drawPixmap(0, 0, pixmap);
}

QT_END_NAMESPACE
