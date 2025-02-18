/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
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

#ifndef QQMLVALUETYPEWRAPPER_P_H
#define QQMLVALUETYPEWRAPPER_P_H

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

#include <QtCore/qglobal.h>
#include <private/qtqmlglobal_p.h>

#include <private/qv4value_p.h>
#include <private/qv4object_p.h>

QT_BEGIN_NAMESPACE

class QQmlValueType;

namespace QV4 {

namespace Heap {

struct QQmlValueTypeWrapper : Object {
    void init() { Object::init(); }
    void destroy();
    QQmlPropertyCache *propertyCache() const { return _propertyCache; }
    void setPropertyCache(QQmlPropertyCache *c) {
        if (c)
            c->addref();
        if (_propertyCache)
            _propertyCache->release();
        _propertyCache = c;
    }
    mutable void *gadgetPtr;
    QQmlValueType *valueType;

    void setValue(const QVariant &value) const;
    QVariant toVariant() const;

private:
    QQmlPropertyCache *_propertyCache;
};

}

struct Q_QML_EXPORT QQmlValueTypeWrapper : Object
{
    V4_OBJECT2(QQmlValueTypeWrapper, Object)
    V4_PROTOTYPE(valueTypeWrapperPrototype)
    V4_NEEDS_DESTROY

public:

    static ReturnedValue create(ExecutionEngine *engine, QObject *, int, const QMetaObject *metaObject, int typeId);
    static ReturnedValue create(ExecutionEngine *engine, const QVariant &, const QMetaObject *metaObject, int typeId);

    QVariant toVariant() const;
    bool toGadget(void *data) const;
    bool isEqual(const QVariant& value);
    int typeId() const;
    bool write(QObject *target, int propertyIndex) const;

    static ReturnedValue get(const Managed *m, String *name, bool *hasProperty);
    static void put(Managed *m, String *name, const Value &value);
    static bool isEqualTo(Managed *m, Managed *other);
    static PropertyAttributes query(const Managed *, String *name);
    static void advanceIterator(Managed *m, ObjectIterator *it, Value *name, uint *index, Property *p, PropertyAttributes *attributes);

    static QV4::ReturnedValue method_toString(CallContext *ctx);

    static void initProto(ExecutionEngine *v4);
};

}

QT_END_NAMESPACE

#endif // QV8VALUETYPEWRAPPER_P_H


