/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef QT3DQUICKVALUETYPES_P_H
#define QT3DQUICKVALUETYPES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qt3dquick_global_p.h>
#include <private/qqmlvaluetype_p.h>

#include <QtGui/QColor>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QQuaternion>
#include <QtGui/QMatrix4x4>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {
namespace Quick3DValueTypes {

QT3DQUICKSHARED_PRIVATE_EXPORT void registerValueTypes();

}

class QT3DQUICKSHARED_PRIVATE_EXPORT Quick3DColorValueType
{
    QColor v;
    Q_PROPERTY(qreal r READ r WRITE setR FINAL)
    Q_PROPERTY(qreal g READ g WRITE setG FINAL)
    Q_PROPERTY(qreal b READ b WRITE setB FINAL)
    Q_PROPERTY(qreal a READ a WRITE setA FINAL)
    Q_GADGET
public:
    Q_INVOKABLE QString toString() const;

    qreal r() const;
    qreal g() const;
    qreal b() const;
    qreal a() const;
    void setR(qreal);
    void setG(qreal);
    void setB(qreal);
    void setA(qreal);
};

class QT3DQUICKSHARED_PRIVATE_EXPORT Quick3DVector2DValueType
{
    QVector2D v;
    Q_PROPERTY(qreal x READ x WRITE setX FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY FINAL)
    Q_GADGET
public:
    Q_INVOKABLE QString toString() const;

    qreal x() const;
    qreal y() const;
    void setX(qreal);
    void setY(qreal);

    Q_INVOKABLE qreal dotProduct(const QVector2D &vec) const;
    Q_INVOKABLE QVector2D times(const QVector2D &vec) const;
    Q_INVOKABLE QVector2D times(qreal scalar) const;
    Q_INVOKABLE QVector2D plus(const QVector2D &vec) const;
    Q_INVOKABLE QVector2D minus(const QVector2D &vec) const;
    Q_INVOKABLE QVector2D normalized() const;
    Q_INVOKABLE qreal length() const;
    Q_INVOKABLE QVector3D toVector3d() const;
    Q_INVOKABLE QVector4D toVector4d() const;
    Q_INVOKABLE bool fuzzyEquals(const QVector2D &vec, qreal epsilon) const;
    Q_INVOKABLE bool fuzzyEquals(const QVector2D &vec) const;
};

class QT3DQUICKSHARED_PRIVATE_EXPORT Quick3DVector3DValueType
{
    QVector3D v;
    Q_PROPERTY(qreal x READ x WRITE setX FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY FINAL)
    Q_PROPERTY(qreal z READ z WRITE setZ FINAL)
    Q_GADGET
public:
    Q_INVOKABLE QString toString() const;

    qreal x() const;
    qreal y() const;
    qreal z() const;
    void setX(qreal);
    void setY(qreal);
    void setZ(qreal);

    Q_INVOKABLE QVector3D crossProduct(const QVector3D &vec) const;
    Q_INVOKABLE qreal dotProduct(const QVector3D &vec) const;
    Q_INVOKABLE QVector3D times(const QMatrix4x4 &m) const;
    Q_INVOKABLE QVector3D times(const QVector3D &vec) const;
    Q_INVOKABLE QVector3D times(qreal scalar) const;
    Q_INVOKABLE QVector3D plus(const QVector3D &vec) const;
    Q_INVOKABLE QVector3D minus(const QVector3D &vec) const;
    Q_INVOKABLE QVector3D normalized() const;
    Q_INVOKABLE qreal length() const;
    Q_INVOKABLE QVector2D toVector2d() const;
    Q_INVOKABLE QVector4D toVector4d() const;
    Q_INVOKABLE bool fuzzyEquals(const QVector3D &vec, qreal epsilon) const;
    Q_INVOKABLE bool fuzzyEquals(const QVector3D &vec) const;
};

class QT3DQUICKSHARED_PRIVATE_EXPORT Quick3DVector4DValueType
{
    QVector4D v;
    Q_PROPERTY(qreal x READ x WRITE setX FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY FINAL)
    Q_PROPERTY(qreal z READ z WRITE setZ FINAL)
    Q_PROPERTY(qreal w READ w WRITE setW FINAL)
    Q_GADGET
public:
    Q_INVOKABLE QString toString() const;

    qreal x() const;
    qreal y() const;
    qreal z() const;
    qreal w() const;
    void setX(qreal);
    void setY(qreal);
    void setZ(qreal);
    void setW(qreal);

    Q_INVOKABLE qreal dotProduct(const QVector4D &vec) const;
    Q_INVOKABLE QVector4D times(const QVector4D &vec) const;
    Q_INVOKABLE QVector4D times(const QMatrix4x4 &m) const;
    Q_INVOKABLE QVector4D times(qreal scalar) const;
    Q_INVOKABLE QVector4D plus(const QVector4D &vec) const;
    Q_INVOKABLE QVector4D minus(const QVector4D &vec) const;
    Q_INVOKABLE QVector4D normalized() const;
    Q_INVOKABLE qreal length() const;
    Q_INVOKABLE QVector2D toVector2d() const;
    Q_INVOKABLE QVector3D toVector3d() const;
    Q_INVOKABLE bool fuzzyEquals(const QVector4D &vec, qreal epsilon) const;
    Q_INVOKABLE bool fuzzyEquals(const QVector4D &vec) const;
};

class QT3DQUICKSHARED_PRIVATE_EXPORT Quick3DQuaternionValueType
{
    QQuaternion v;
    Q_PROPERTY(qreal scalar READ scalar WRITE setScalar)
    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(qreal z READ z WRITE setZ)
    Q_GADGET
public:
    Q_INVOKABLE QString toString() const;

    qreal scalar() const;
    qreal x() const;
    qreal y() const;
    qreal z() const;
    void setScalar(qreal);
    void setX(qreal);
    void setY(qreal);
    void setZ(qreal);
};

class QT3DQUICKSHARED_PRIVATE_EXPORT Quick3DMatrix4x4ValueType
{
    QMatrix4x4 v;
    Q_PROPERTY(qreal m11 READ m11 WRITE setM11 FINAL)
    Q_PROPERTY(qreal m12 READ m12 WRITE setM12 FINAL)
    Q_PROPERTY(qreal m13 READ m13 WRITE setM13 FINAL)
    Q_PROPERTY(qreal m14 READ m14 WRITE setM14 FINAL)
    Q_PROPERTY(qreal m21 READ m21 WRITE setM21 FINAL)
    Q_PROPERTY(qreal m22 READ m22 WRITE setM22 FINAL)
    Q_PROPERTY(qreal m23 READ m23 WRITE setM23 FINAL)
    Q_PROPERTY(qreal m24 READ m24 WRITE setM24 FINAL)
    Q_PROPERTY(qreal m31 READ m31 WRITE setM31 FINAL)
    Q_PROPERTY(qreal m32 READ m32 WRITE setM32 FINAL)
    Q_PROPERTY(qreal m33 READ m33 WRITE setM33 FINAL)
    Q_PROPERTY(qreal m34 READ m34 WRITE setM34 FINAL)
    Q_PROPERTY(qreal m41 READ m41 WRITE setM41 FINAL)
    Q_PROPERTY(qreal m42 READ m42 WRITE setM42 FINAL)
    Q_PROPERTY(qreal m43 READ m43 WRITE setM43 FINAL)
    Q_PROPERTY(qreal m44 READ m44 WRITE setM44 FINAL)
    Q_GADGET
public:
    Q_INVOKABLE QString toString() const;

    qreal m11() const { return v(0, 0); }
    qreal m12() const { return v(0, 1); }
    qreal m13() const { return v(0, 2); }
    qreal m14() const { return v(0, 3); }
    qreal m21() const { return v(1, 0); }
    qreal m22() const { return v(1, 1); }
    qreal m23() const { return v(1, 2); }
    qreal m24() const { return v(1, 3); }
    qreal m31() const { return v(2, 0); }
    qreal m32() const { return v(2, 1); }
    qreal m33() const { return v(2, 2); }
    qreal m34() const { return v(2, 3); }
    qreal m41() const { return v(3, 0); }
    qreal m42() const { return v(3, 1); }
    qreal m43() const { return v(3, 2); }
    qreal m44() const { return v(3, 3); }

    void setM11(qreal value) { v(0, 0) = value; }
    void setM12(qreal value) { v(0, 1) = value; }
    void setM13(qreal value) { v(0, 2) = value; }
    void setM14(qreal value) { v(0, 3) = value; }
    void setM21(qreal value) { v(1, 0) = value; }
    void setM22(qreal value) { v(1, 1) = value; }
    void setM23(qreal value) { v(1, 2) = value; }
    void setM24(qreal value) { v(1, 3) = value; }
    void setM31(qreal value) { v(2, 0) = value; }
    void setM32(qreal value) { v(2, 1) = value; }
    void setM33(qreal value) { v(2, 2) = value; }
    void setM34(qreal value) { v(2, 3) = value; }
    void setM41(qreal value) { v(3, 0) = value; }
    void setM42(qreal value) { v(3, 1) = value; }
    void setM43(qreal value) { v(3, 2) = value; }
    void setM44(qreal value) { v(3, 3) = value; }

    Q_INVOKABLE void translate(float x, float y, float z) { v.translate(x, y, z); }
    Q_INVOKABLE void translate(const QVector3D &t) { v.translate(t); }
    Q_INVOKABLE void rotate(float angle, const QVector3D &axis) { v.rotate(angle, axis); }
    Q_INVOKABLE void scale(float s) { v.scale(s); }
    Q_INVOKABLE void scale(float sx, float sy, float sz) { v.scale(sx, sy, sz); }
    Q_INVOKABLE void scale(const QVector3D &s) { v.scale(s); }
    Q_INVOKABLE void lookAt(const QVector3D &eye, const QVector3D &center, const QVector3D &up) { v.lookAt(eye, center, up); }

    Q_INVOKABLE QMatrix4x4 times(const QMatrix4x4 &m) const;
    Q_INVOKABLE QVector4D times(const QVector4D &vec) const;
    Q_INVOKABLE QVector3D times(const QVector3D &vec) const;
    Q_INVOKABLE QMatrix4x4 times(qreal factor) const;
    Q_INVOKABLE QMatrix4x4 plus(const QMatrix4x4 &m) const;
    Q_INVOKABLE QMatrix4x4 minus(const QMatrix4x4 &m) const;

    Q_INVOKABLE QVector4D row(int n) const;
    Q_INVOKABLE QVector4D column(int m) const;

    Q_INVOKABLE qreal determinant() const;
    Q_INVOKABLE QMatrix4x4 inverted() const;
    Q_INVOKABLE QMatrix4x4 transposed() const;

    Q_INVOKABLE bool fuzzyEquals(const QMatrix4x4 &m, qreal epsilon) const;
    Q_INVOKABLE bool fuzzyEquals(const QMatrix4x4 &m) const;
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DQUICKVALUETYPES_P_H
