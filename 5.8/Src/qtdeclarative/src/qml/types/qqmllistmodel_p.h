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

#ifndef QQMLLISTMODEL_H
#define QQMLLISTMODEL_H

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

#include <qqml.h>
#include <private/qqmlcustomparser_p.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/qabstractitemmodel.h>

#include <private/qv4engine_p.h>
#include <private/qpodvector_p.h>

QT_BEGIN_NAMESPACE


class QQmlListModelWorkerAgent;
class ListModel;
class ListLayout;

namespace QV4 {
struct ModelObject;
}

class Q_QML_PRIVATE_EXPORT QQmlListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool dynamicRoles READ dynamicRoles WRITE setDynamicRoles)

public:
    QQmlListModel(QObject *parent=0);
    ~QQmlListModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QHash<int,QByteArray> roleNames() const;

    QVariant data(int index, int role) const;
    int count() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(QQmlV4Function *args);
    Q_INVOKABLE void append(QQmlV4Function *args);
    Q_INVOKABLE void insert(QQmlV4Function *args);
    Q_INVOKABLE QQmlV4Handle get(int index) const;
    Q_INVOKABLE void set(int index, const QQmlV4Handle &);
    Q_INVOKABLE void setProperty(int index, const QString& property, const QVariant& value);
    Q_INVOKABLE void move(int from, int to, int count);
    Q_INVOKABLE void sync();

    QQmlListModelWorkerAgent *agent();

    bool dynamicRoles() const { return m_dynamicRoles; }
    void setDynamicRoles(bool enableDynamicRoles);

Q_SIGNALS:
    void countChanged();

private:
    friend class QQmlListModelParser;
    friend class QQmlListModelWorkerAgent;
    friend class ModelObject;
    friend struct QV4::ModelObject;
    friend class ModelNodeMetaObject;
    friend class ListModel;
    friend class ListElement;
    friend class DynamicRoleModelNode;
    friend class DynamicRoleModelNodeMetaObject;

    // Constructs a flat list model for a worker agent
    QQmlListModel(QQmlListModel *orig, QQmlListModelWorkerAgent *agent);
    QQmlListModel(const QQmlListModel *owner, ListModel *data, QV4::ExecutionEngine *engine, QObject *parent=0);

    QV4::ExecutionEngine *engine() const;

    inline bool canMove(int from, int to, int n) const { return !(from+n > count() || to+n > count() || from < 0 || to < 0 || n < 0); }

    QQmlListModelWorkerAgent *m_agent;
    mutable QV4::ExecutionEngine *m_engine;
    bool m_mainThread;
    bool m_primary;

    bool m_dynamicRoles;

    ListLayout *m_layout;
    ListModel *m_listModel;

    QVector<class DynamicRoleModelNode *> m_modelObjects;
    QVector<QString> m_roles;
    int m_uid;

    struct ElementSync
    {
        ElementSync() : src(0), target(0) {}

        DynamicRoleModelNode *src;
        DynamicRoleModelNode *target;
    };

    int getUid() const { return m_uid; }

    static void sync(QQmlListModel *src, QQmlListModel *target, QHash<int, QQmlListModel *> *targetModelHash);
    static QQmlListModel *createWithOwner(QQmlListModel *newOwner);

    void emitItemsChanged(int index, int count, const QVector<int> &roles);
    void emitItemsAboutToBeRemoved(int index, int count);
    void emitItemsRemoved(int index, int count);
    void emitItemsAboutToBeInserted(int index, int count);
    void emitItemsInserted(int index, int count);
    void emitItemsAboutToBeMoved(int from, int to, int n);
    void emitItemsMoved(int from, int to, int n);
};

// ### FIXME
class QQmlListElement : public QObject
{
Q_OBJECT
};

class QQmlListModelParser : public QQmlCustomParser
{
public:
    enum PropertyType {
        Invalid,
        Boolean,
        Number,
        String,
        Script
    };


    QQmlListModelParser() : QQmlCustomParser(QQmlCustomParser::AcceptsSignalHandlers) {}

    void verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings) Q_DECL_OVERRIDE;
    void applyBindings(QObject *obj, QV4::CompiledData::CompilationUnit *compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) Q_DECL_OVERRIDE;

private:
    bool verifyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding);
    // returns true if a role was set
    bool applyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding, ListModel *model, int outterElementIndex);

    bool definesEmptyList(const QString &);

    QString listElementTypeName;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQmlListModel)
QML_DECLARE_TYPE(QQmlListElement)

#endif // QQMLLISTMODEL_H
