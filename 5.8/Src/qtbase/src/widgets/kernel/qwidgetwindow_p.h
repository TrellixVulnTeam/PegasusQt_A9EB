/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QWIDGETWINDOW_P_H
#define QWIDGETWINDOW_P_H

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

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include <QtGui/qwindow.h>

#include <QtCore/private/qobject_p.h>
#include <QtGui/private/qevent_p.h>

QT_BEGIN_NAMESPACE


class QCloseEvent;
class QMoveEvent;

class QWidgetWindow : public QWindow
{
    Q_OBJECT
public:
    QWidgetWindow(QWidget *widget);
    ~QWidgetWindow();

    QWidget *widget() const { return m_widget; }
#ifndef QT_NO_ACCESSIBILITY
    QAccessibleInterface *accessibleRoot() const Q_DECL_OVERRIDE;
#endif

    QObject *focusObject() const Q_DECL_OVERRIDE;
protected:
    bool event(QEvent *) Q_DECL_OVERRIDE;

    void handleCloseEvent(QCloseEvent *);
    void handleEnterLeaveEvent(QEvent *);
    void handleFocusInEvent(QFocusEvent *);
    void handleKeyEvent(QKeyEvent *);
    void handleMouseEvent(QMouseEvent *);
    void handleNonClientAreaMouseEvent(QMouseEvent *);
    void handleTouchEvent(QTouchEvent *);
    void handleMoveEvent(QMoveEvent *);
    void handleResizeEvent(QResizeEvent *);
#ifndef QT_NO_WHEELEVENT
    void handleWheelEvent(QWheelEvent *);
#endif
#ifndef QT_NO_DRAGANDDROP
    void handleDragEnterMoveEvent(QDragMoveEvent *);
    void handleDragLeaveEvent(QDragLeaveEvent *);
    void handleDropEvent(QDropEvent *);
#endif
    void handleExposeEvent(QExposeEvent *);
    void handleWindowStateChangedEvent(QWindowStateChangeEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;
#ifndef QT_NO_TABLETEVENT
    void handleTabletEvent(QTabletEvent *);
#endif
#ifndef QT_NO_GESTURES
    void handleGestureEvent(QNativeGestureEvent *);
#endif
#ifndef QT_NO_CONTEXTMENU
    void handleContextMenuEvent(QContextMenuEvent *);
#endif

private slots:
    void updateObjectName();
    void handleScreenChange();

private:
    void repaintWindow();
    bool updateSize();
    bool updatePos();
    void updateMargins();
    void updateNormalGeometry();

    enum FocusWidgets {
        FirstFocusWidget,
        LastFocusWidget
    };
    QWidget *getFocusWidget(FocusWidgets fw);

    QPointer<QWidget> m_widget;
    QPointer<QWidget> m_implicit_mouse_grabber;
#ifndef QT_NO_DRAGANDDROP
    QPointer<QWidget> m_dragTarget;
#endif
};

QT_END_NAMESPACE

#endif // QWIDGETWINDOW_P_H
