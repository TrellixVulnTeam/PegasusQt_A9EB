/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQuick module of the Qt Toolkit.
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

#ifndef QQUICKSHORTCUT_P_H
#define QQUICKSHORTCUT_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qkeysequence.h>
#include <QtQml/qqmlparserstatus.h>

QT_BEGIN_NAMESPACE

class QQuickShortcut : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QVariant sequence READ sequence WRITE setSequence NOTIFY sequenceChanged FINAL)
    Q_PROPERTY(QString nativeText READ nativeText NOTIFY sequenceChanged FINAL REVISION 1)
    Q_PROPERTY(QString portableText READ portableText NOTIFY sequenceChanged FINAL REVISION 1)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(bool autoRepeat READ autoRepeat WRITE setAutoRepeat NOTIFY autoRepeatChanged FINAL)
    Q_PROPERTY(Qt::ShortcutContext context READ context WRITE setContext NOTIFY contextChanged FINAL)

public:
    explicit QQuickShortcut(QObject *parent = Q_NULLPTR);
    ~QQuickShortcut();

    QVariant sequence() const;
    void setSequence(const QVariant &sequence);

    QString nativeText() const;
    QString portableText() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool autoRepeat() const;
    void setAutoRepeat(bool repeat);

    Qt::ShortcutContext context() const;
    void setContext(Qt::ShortcutContext context);

Q_SIGNALS:
    void sequenceChanged();
    void enabledChanged();
    void autoRepeatChanged();
    void contextChanged();

    void activated();
    void activatedAmbiguously();

protected:
    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    void grabShortcut(const QKeySequence &sequence, Qt::ShortcutContext context);
    void ungrabShortcut();

private:
    int m_id;
    bool m_enabled;
    bool m_completed;
    bool m_autorepeat;
    QKeySequence m_shortcut;
    Qt::ShortcutContext m_context;
    QVariant m_sequence;
};

QT_END_NAMESPACE

#endif // QQUICKSHORTCUT_P_H
