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
#ifndef QMLJS_MANAGED_H
#define QMLJS_MANAGED_H

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

#include "qv4global_p.h"
#include "qv4value_p.h"
#include <private/qv4heap_p.h>

QT_BEGIN_NAMESPACE

namespace QV4 {

#define Q_MANAGED_CHECK \
    template <typename Type> inline void qt_check_for_QMANAGED_macro(const Type *_q_argument) const \
    { int i = qYouForgotTheQ_MANAGED_Macro(this, _q_argument); i = i + 1; }

template <typename T>
inline int qYouForgotTheQ_MANAGED_Macro(T, T) { return 0; }

template <typename T1, typename T2>
inline void qYouForgotTheQ_MANAGED_Macro(T1, T2) {}

#ifdef Q_COMPILER_STATIC_ASSERT
#define V4_MANAGED_SIZE_TEST void __dataTest() { Q_STATIC_ASSERT(sizeof(*this) == sizeof(Managed)); }
#else
#define V4_MANAGED_SIZE_TEST
#endif

#define V4_NEEDS_DESTROY static void destroy(QV4::Heap::Base *b) { static_cast<Data *>(b)->destroy(); }


#define V4_MANAGED_ITSELF(DataClass, superClass) \
    public: \
        Q_MANAGED_CHECK \
        typedef QV4::Heap::DataClass Data; \
        typedef superClass SuperClass; \
        static const QV4::VTable static_vtbl; \
        static inline const QV4::VTable *staticVTable() { return &static_vtbl; } \
        V4_MANAGED_SIZE_TEST \
        QV4::Heap::DataClass *d_unchecked() const { return static_cast<QV4::Heap::DataClass *>(m()); } \
        QV4::Heap::DataClass *d() const { \
            QV4::Heap::DataClass *dptr = d_unchecked(); \
            dptr->_checkIsInitialized(); \
            return dptr; \
        } \
        V4_ASSERT_IS_TRIVIAL(QV4::Heap::DataClass)

#define V4_MANAGED(DataClass, superClass) \
    private: \
        DataClass() Q_DECL_EQ_DELETE; \
        Q_DISABLE_COPY(DataClass) \
        V4_MANAGED_ITSELF(DataClass, superClass)

#define Q_MANAGED_TYPE(type) \
    public: \
        enum { MyType = Type_##type };

#define Q_VTABLE_FUNCTION(classname, func) \
    (classname::func == QV4::Managed::func ? 0 : classname::func)

// Q_VTABLE_FUNCTION triggers a bogus tautological-compare warning in GCC6+
#if (defined(Q_CC_GNU) && Q_CC_GNU >= 600)
#define QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_ON \
    QT_WARNING_PUSH; \
    QT_WARNING_DISABLE_GCC("-Wtautological-compare")

#define QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_OFF \
    ;QT_WARNING_POP
#elif defined(Q_CC_CLANG) && Q_CC_CLANG >= 306
#define QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_ON \
    QT_WARNING_PUSH; \
    QT_WARNING_DISABLE_CLANG("-Wtautological-compare")

#define QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_OFF \
    ;QT_WARNING_POP
#else
#define QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_ON
#define QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_OFF
#endif

#define DEFINE_MANAGED_VTABLE_INT(classname, parentVTable) \
{     \
    parentVTable, \
    classname::IsExecutionContext,   \
    classname::IsString,   \
    classname::IsObject,   \
    classname::IsFunctionObject,   \
    classname::IsErrorObject,   \
    classname::IsArrayData,   \
    0,                                          \
    classname::MyType,                          \
    #classname, \
    Q_VTABLE_FUNCTION(classname, destroy),                                    \
    markObjects,                                \
    isEqualTo                                  \
}

#define DEFINE_MANAGED_VTABLE(classname) \
QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_ON \
const QV4::VTable classname::static_vtbl = DEFINE_MANAGED_VTABLE_INT(classname, 0) \
QT_WARNING_SUPPRESS_GCC_TAUTOLOGICAL_COMPARE_OFF

struct Q_QML_PRIVATE_EXPORT Managed : Value
{
    V4_MANAGED_ITSELF(Base, Managed)
    enum {
        IsExecutionContext = false,
        IsString = false,
        IsObject = false,
        IsFunctionObject = false,
        IsErrorObject = false,
        IsArrayData = false
    };
private:
    void *operator new(size_t);
    Managed() Q_DECL_EQ_DELETE;
    Q_DISABLE_COPY(Managed)

public:

    enum Type {
        Type_Invalid,
        Type_String,
        Type_Object,
        Type_ArrayObject,
        Type_FunctionObject,
        Type_BooleanObject,
        Type_NumberObject,
        Type_StringObject,
        Type_DateObject,
        Type_RegExpObject,
        Type_ErrorObject,
        Type_ArgumentsObject,
        Type_JsonObject,
        Type_MathObject,

        Type_ExecutionContext,
        Type_ForeachIteratorObject,
        Type_RegExp,

        Type_QmlSequence
    };
    Q_MANAGED_TYPE(Invalid)

    bool isListType() const { return d()->vtable()->type == Type_QmlSequence; }

    bool isArrayObject() const { return d()->vtable()->type == Type_ArrayObject; }
    bool isStringObject() const { return d()->vtable()->type == Type_StringObject; }

    QString className() const;

    bool isEqualTo(const Managed *other) const
    { return d()->vtable()->isEqualTo(const_cast<Managed *>(this), const_cast<Managed *>(other)); }

    static bool isEqualTo(Managed *m, Managed *other);

    bool inUse() const { return d()->inUse(); }
    bool markBit() const { return d()->isMarked(); }

    static void destroy(Heap::Base *) {}
private:
    friend class MemoryManager;
    friend struct Identifiers;
    friend struct ObjectIterator;
};


template<>
inline const Managed *Value::as() const {
    if (isManaged())
        return managed();
    return 0;
}

template<>
inline const Object *Value::as() const {
    return isManaged() && m() && m()->vtable()->isObject ? objectValue() : 0;
}

}


QT_END_NAMESPACE

#endif
