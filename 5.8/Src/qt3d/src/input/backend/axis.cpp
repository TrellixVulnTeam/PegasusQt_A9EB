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

#include "axis_p.h"
#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qabstractaxisinput.h>
#include <Qt3DInput/private/qaxis_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

Axis::Axis()
    : Qt3DCore::QBackendNode(ReadWrite)
    , m_axisValue(0.0f)
{
}

void Axis::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QAxisData>>(change);
    const auto &data = typedChange->data;
    m_inputs = data.inputIds;
}

void Axis::cleanup()
{
    QBackendNode::setEnabled(false);
    m_inputs.clear();
    m_axisValue = 0.0f;
}

void Axis::setAxisValue(float axisValue)
{
    if (isEnabled() && (axisValue != m_axisValue)) {
        m_axisValue = axisValue;

        // Send a change to the frontend
        auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
        e->setPropertyName("value");
        e->setValue(m_axisValue);
        notifyObservers(e);
    }
}

void Axis::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    switch (e->type()) {
    case Qt3DCore::PropertyValueAdded: {
        const auto change = qSharedPointerCast<Qt3DCore::QPropertyNodeAddedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("input"))
            m_inputs.push_back(change->addedNodeId());
        break;
    }

    case Qt3DCore::PropertyValueRemoved: {
        const auto change = qSharedPointerCast<Qt3DCore::QPropertyNodeRemovedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("input"))
            m_inputs.removeOne(change->removedNodeId());
    }

    default:
        break;
    }
    QBackendNode::sceneChangeEvent(e);
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE
