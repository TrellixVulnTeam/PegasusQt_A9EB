/****************************************************************************
**
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

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DMATERIAL_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DMATERIAL_P_H

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

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qparameter.h>
#include <QQmlListProperty>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

// FIXME - write a custom QML parser and stop mis-using Tag
// Tags could be replaced by Parameters directly

class QT3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DMaterial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QParameter> parameters READ qmlParameters)

public:
    explicit Quick3DMaterial(QObject *parent = 0);

    // TO DO : replace by QAbstractMaterial later on
    inline QMaterial *parentMaterial() const { return qobject_cast<QMaterial*>(parent()); }

    QQmlListProperty<QParameter> qmlParameters();


private:
    // FIXME - remove when we have a custom QML parser
    static void appendParameter(QQmlListProperty<QParameter> *list, QParameter *bar);
    static QParameter *parameterAt(QQmlListProperty<QParameter> *list, int index);
    static int parameterCount(QQmlListProperty<QParameter> *list);
    static void clearParameters(QQmlListProperty<QParameter> *list);
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DMATERIAL_P_H
