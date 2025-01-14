/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#ifndef QGEORECTANGLE_H
#define QGEORECTANGLE_H

#include <QtPositioning/QGeoShape>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoRectanglePrivate;

class Q_POSITIONING_EXPORT QGeoRectangle : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QGeoCoordinate bottomLeft READ bottomLeft WRITE setBottomLeft)
    Q_PROPERTY(QGeoCoordinate bottomRight READ bottomRight WRITE setBottomRight)
    Q_PROPERTY(QGeoCoordinate topLeft READ topLeft WRITE setTopLeft)
    Q_PROPERTY(QGeoCoordinate topRight READ topRight WRITE setTopRight)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter)
    Q_PROPERTY(double height READ height WRITE setHeight)
    Q_PROPERTY(double width READ width WRITE setWidth)

public:
    QGeoRectangle();
    QGeoRectangle(const QGeoCoordinate &center, double degreesWidth, double degreesHeight);
    QGeoRectangle(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
    QGeoRectangle(const QList<QGeoCoordinate> &coordinates);
    QGeoRectangle(const QGeoRectangle &other);
    QGeoRectangle(const QGeoShape &other);

    ~QGeoRectangle();

    QGeoRectangle &operator=(const QGeoRectangle &other);

    using QGeoShape::operator==;
    bool operator==(const QGeoRectangle &other) const;

    using QGeoShape::operator!=;
    bool operator!=(const QGeoRectangle &other) const;

    void setTopLeft(const QGeoCoordinate &topLeft);
    QGeoCoordinate topLeft() const;

    void setTopRight(const QGeoCoordinate &topRight);
    QGeoCoordinate topRight() const;

    void setBottomLeft(const QGeoCoordinate &bottomLeft);
    QGeoCoordinate bottomLeft() const;

    void setBottomRight(const QGeoCoordinate &bottomRight);
    QGeoCoordinate bottomRight() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setWidth(double degreesWidth);
    double width() const;

    void setHeight(double degreesHeight);
    double height() const;

    using QGeoShape::contains;
    bool contains(const QGeoRectangle &rectangle) const;
    bool intersects(const QGeoRectangle &rectangle) const;

    void translate(double degreesLatitude, double degreesLongitude);
    QGeoRectangle translated(double degreesLatitude, double degreesLongitude) const;

    QGeoRectangle united(const QGeoRectangle &rectangle) const;
    QGeoRectangle operator|(const QGeoRectangle &rectangle) const;
    QGeoRectangle &operator|=(const QGeoRectangle &rectangle);

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoRectanglePrivate *d_func();
    inline const QGeoRectanglePrivate *d_func() const;
};

Q_DECLARE_TYPEINFO(QGeoRectangle, Q_MOVABLE_TYPE);

inline QGeoRectangle QGeoRectangle::operator|(const QGeoRectangle &rectangle) const
{
    return united(rectangle);
}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoRectangle)

#endif

