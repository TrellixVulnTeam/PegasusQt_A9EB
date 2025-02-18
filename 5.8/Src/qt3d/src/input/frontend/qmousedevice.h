/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DINPUT_QMOUSEDEVICE_H
#define QT3DINPUT_QMOUSEDEVICE_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QMouseDevicePrivate;
class QMouseHandler;

class QMouseInput;


class QT3DINPUTSHARED_EXPORT QMouseDevice : public Qt3DInput::QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(float sensitivity READ sensitivity WRITE setSensitivity NOTIFY sensitivityChanged)
public:
    explicit QMouseDevice(Qt3DCore::QNode *parent = nullptr);
    ~QMouseDevice();

    enum Axis {
        X,
        Y,
        WheelX,
        WheelY
    };
    Q_ENUM(Axis) // LCOV_EXCL_LINE

    int axisCount() const Q_DECL_FINAL;
    int buttonCount() const Q_DECL_FINAL;
    QStringList axisNames() const Q_DECL_FINAL;
    QStringList buttonNames() const Q_DECL_FINAL;
    int axisIdentifier(const QString &name) const Q_DECL_FINAL;
    int buttonIdentifier(const QString &name) const Q_DECL_FINAL;

    float sensitivity() const;

public Q_SLOTS:
    void setSensitivity(float value);

Q_SIGNALS:
    void sensitivityChanged(float value);

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QMouseDevice)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const Q_DECL_OVERRIDE;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DInput::QMouseDevice*) // LCOV_EXCL_LINE

#endif // QT3DINPUT_QMOUSEDEVICE_H
