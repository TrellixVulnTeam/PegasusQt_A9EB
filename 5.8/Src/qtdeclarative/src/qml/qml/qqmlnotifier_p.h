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

#ifndef QQMLNOTIFIER_P_H
#define QQMLNOTIFIER_P_H

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

#include "qqmldata_p.h"
#include <QtCore/qmetaobject.h>
#include <private/qmetaobject_p.h>

QT_BEGIN_NAMESPACE

class QQmlNotifierEndpoint;
class Q_QML_PRIVATE_EXPORT QQmlNotifier
{
public:
    inline QQmlNotifier();
    inline ~QQmlNotifier();
    inline void notify();

    static void notify(QQmlData *ddata, int notifierIndex);

private:
    friend class QQmlData;
    friend class QQmlNotifierEndpoint;
    friend class QQmlThreadNotifierProxyObject;

    static void emitNotify(QQmlNotifierEndpoint *, void **a);
    QQmlNotifierEndpoint *endpoints;
};

class QQmlEngine;
class QQmlNotifierEndpoint
{
    QQmlNotifierEndpoint  *next;
    QQmlNotifierEndpoint **prev;
public:
    // QQmlNotifierEndpoint can only invoke one of a set of pre-defined callbacks.
    // To add another callback, extend this enum and add the callback to the top
    // of qqmlnotifier.cpp.  Four bits are reserved for the callback, so there can
    // be up to 15 of them (0 is reserved).
    enum Callback {
        None = 0,
        QQmlBoundSignal = 1,
        QQmlJavaScriptExpressionGuard = 2,
        QQmlVMEMetaObjectEndpoint = 3
    };

    inline QQmlNotifierEndpoint(Callback callback);
    inline ~QQmlNotifierEndpoint();

    inline bool isConnected();
    inline bool isConnected(QObject *source, int sourceSignal);
    inline bool isConnected(QQmlNotifier *);

    void connect(QObject *source, int sourceSignal, QQmlEngine *engine);
    inline void connect(QQmlNotifier *);
    inline void disconnect();

    inline bool isNotifying() const;
    inline void cancelNotify();

    inline int signalIndex() const { return sourceSignal; }

private:
    friend class QQmlData;
    friend class QQmlNotifier;

    // Contains either the QObject*, or the QQmlNotifier* that this
    // endpoint is connected to.  While the endpoint is notifying, the
    // senderPtr points to another qintptr that contains this value.
    qintptr senderPtr;
    inline QObject *senderAsObject() const;
    inline QQmlNotifier *senderAsNotifier() const;

    Callback callback:4;
    // The index is in the range returned by QObjectPrivate::signalIndex().
    // This is different from QMetaMethod::methodIndex().
    signed int sourceSignal:28;
};

QQmlNotifier::QQmlNotifier()
: endpoints(0)
{
}

QQmlNotifier::~QQmlNotifier()
{
    QQmlNotifierEndpoint *endpoint = endpoints;
    while (endpoint) {
        QQmlNotifierEndpoint *n = endpoint;
        endpoint = n->next;

        if (n->isNotifying()) *((qintptr *)(n->senderPtr & ~0x1)) = 0;

        n->next = 0;
        n->prev = 0;
        n->senderPtr = 0;
        n->sourceSignal = -1;
    }
    endpoints = 0;
}

void QQmlNotifier::notify()
{
    void *args[] = { 0 };
    if (endpoints) emitNotify(endpoints, args);
}

QQmlNotifierEndpoint::QQmlNotifierEndpoint(Callback callback)
: next(0), prev(0), senderPtr(0), callback(callback), sourceSignal(-1)
{
}

QQmlNotifierEndpoint::~QQmlNotifierEndpoint()
{
    disconnect();
}

bool QQmlNotifierEndpoint::isConnected()
{
    return prev != 0;
}

/*! \internal
    \a sourceSignal MUST be in the signal index range (see QObjectPrivate::signalIndex()).
    This is different from QMetaMethod::methodIndex().
*/
bool QQmlNotifierEndpoint::isConnected(QObject *source, int sourceSignal)
{
    return this->sourceSignal != -1 && senderAsObject() == source &&
           this->sourceSignal == sourceSignal;
}

bool QQmlNotifierEndpoint::isConnected(QQmlNotifier *notifier)
{
    return sourceSignal == -1 && senderAsNotifier() == notifier;
}

void QQmlNotifierEndpoint::connect(QQmlNotifier *notifier)
{
    disconnect();

    next = notifier->endpoints;
    if (next) { next->prev = &next; }
    notifier->endpoints = this;
    prev = &notifier->endpoints;
    senderPtr = qintptr(notifier);
}

void QQmlNotifierEndpoint::disconnect()
{
    // Remove from notifier chain before calling disconnectNotify(), so that that
    // QObject::receivers() returns the correct value in there
    if (next) next->prev = prev;
    if (prev) *prev = next;

    if (sourceSignal != -1) {
        QObject * const obj = senderAsObject();
        QObjectPrivate * const priv = QObjectPrivate::get(obj);
        priv->disconnectNotify(QMetaObjectPrivate::signal(obj->metaObject(), sourceSignal));
    }

    if (isNotifying()) *((qintptr *)(senderPtr & ~0x1)) = 0;
    next = 0;
    prev = 0;
    senderPtr = 0;
    sourceSignal = -1;
}

/*!
Returns true if a notify is in progress.  This means that the signal or QQmlNotifier
that this endpoing is connected to has been triggered, but this endpoint's callback has not
yet been called.

An in progress notify can be cancelled by calling cancelNotify.
*/
bool QQmlNotifierEndpoint::isNotifying() const
{
    return senderPtr & 0x1;
}

/*!
Cancel any notifies that are in progress.
*/
void QQmlNotifierEndpoint::cancelNotify()
{
    if (isNotifying()) {
        qintptr sp = *((qintptr *)(senderPtr & ~0x1));
        *((qintptr *)(senderPtr & ~0x1)) = 0;
        senderPtr = sp;
    }
}

QObject *QQmlNotifierEndpoint::senderAsObject() const
{
    return isNotifying()?((QObject *)(*((qintptr *)(senderPtr & ~0x1)))):((QObject *)senderPtr);
}

QQmlNotifier *QQmlNotifierEndpoint::senderAsNotifier() const
{
    return isNotifying()?((QQmlNotifier *)(*((qintptr *)(senderPtr & ~0x1)))):((QQmlNotifier *)senderPtr);
}

QT_END_NAMESPACE

#endif // QQMLNOTIFIER_P_H

