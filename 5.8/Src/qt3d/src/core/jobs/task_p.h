/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#ifndef QT3DCORE_TASK_P_H
#define QT3DCORE_TASK_P_H

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

#include "qaspectjobmanager_p.h"

#include <QtCore/QtGlobal>
#include <QtCore/QThread>
#include <QtCore/QSharedPointer>

#include <QtCore/QRunnable>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class JobRunner;
class DependencyHandler;
class QThreadPooler;

class RunnableInterface : public QRunnable
{
public:
    virtual ~RunnableInterface();

    virtual void run() = 0;

    virtual void setDependencyHandler(DependencyHandler *) = 0;
    virtual DependencyHandler *dependencyHandler() = 0;

    virtual int id() = 0;
    virtual void setId(int id) = 0;

    virtual void setReserved(bool reserved) = 0;
    virtual bool reserved() = 0;

    virtual void setPooler(QThreadPooler *pooler) = 0;
};

class AspectTaskRunnable : public RunnableInterface
{
public:
    AspectTaskRunnable();
    ~AspectTaskRunnable();

    void run() Q_DECL_OVERRIDE;

    void setDependencyHandler(DependencyHandler *handler) Q_DECL_OVERRIDE;
    DependencyHandler *dependencyHandler() Q_DECL_OVERRIDE;

    void setPooler(QThreadPooler *pooler) Q_DECL_OVERRIDE { m_pooler = pooler; }

    void setReserved(bool reserved) Q_DECL_OVERRIDE { m_reserved = reserved; }
    bool reserved() Q_DECL_OVERRIDE { return m_reserved; }

    int id() Q_DECL_OVERRIDE { return m_id; }
    void setId(int id) Q_DECL_OVERRIDE { m_id = id; }

public:
    QSharedPointer<QAspectJob> m_job;

private:
    DependencyHandler *m_dependencyHandler;
    QThreadPooler *m_pooler;
    bool m_reserved;

    int m_id; // For testing purposes for now
};

class SyncTaskRunnable : public RunnableInterface
{
public:
    explicit SyncTaskRunnable(QAbstractAspectJobManager::JobFunction func, void *arg,
                              QAtomicInt *atomicCount);
    ~SyncTaskRunnable();

    void run() Q_DECL_OVERRIDE;

    void setDependencyHandler(DependencyHandler *handler) Q_DECL_OVERRIDE;
    DependencyHandler *dependencyHandler() Q_DECL_OVERRIDE;

    void setPooler(QThreadPooler *pooler) Q_DECL_OVERRIDE { m_pooler = pooler; }

    void setReserved(bool reserved) Q_DECL_OVERRIDE { m_reserved = reserved; }
    bool reserved() Q_DECL_OVERRIDE { return m_reserved; }

    int id() Q_DECL_OVERRIDE { return m_id; }
    void setId(int id) Q_DECL_OVERRIDE { m_id = id; }

private:
    QAbstractAspectJobManager::JobFunction m_func;
    void *m_arg;
    QAtomicInt *m_atomicCount;

    QThreadPooler *m_pooler;
    bool m_reserved;

    int m_id;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_TASK_P_H

