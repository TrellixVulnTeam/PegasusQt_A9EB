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

#ifndef QTOOLBUTTON_H
#define QTOOLBUTTON_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qabstractbutton.h>

QT_BEGIN_NAMESPACE


#ifndef QT_NO_TOOLBUTTON

class QToolButtonPrivate;
class QMenu;
class QStyleOptionToolButton;

class Q_WIDGETS_EXPORT QToolButton : public QAbstractButton
{
    Q_OBJECT
    Q_ENUMS(Qt::ToolButtonStyle Qt::ArrowType)
#ifndef QT_NO_MENU
    Q_PROPERTY(ToolButtonPopupMode popupMode READ popupMode WRITE setPopupMode)
#endif
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
    Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
    Q_PROPERTY(Qt::ArrowType arrowType READ arrowType WRITE setArrowType)

public:
    enum ToolButtonPopupMode {
        DelayedPopup,
        MenuButtonPopup,
        InstantPopup
    };
    Q_ENUM(ToolButtonPopupMode)

    explicit QToolButton(QWidget *parent = Q_NULLPTR);
    ~QToolButton();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    Qt::ToolButtonStyle toolButtonStyle() const;

    Qt::ArrowType arrowType() const;
    void setArrowType(Qt::ArrowType type);

#ifndef QT_NO_MENU
    void setMenu(QMenu* menu);
    QMenu* menu() const;

    void setPopupMode(ToolButtonPopupMode mode);
    ToolButtonPopupMode popupMode() const;
#endif

    QAction *defaultAction() const;

    void setAutoRaise(bool enable);
    bool autoRaise() const;

public Q_SLOTS:
#ifndef QT_NO_MENU
    void showMenu();
#endif
    void setToolButtonStyle(Qt::ToolButtonStyle style);
    void setDefaultAction(QAction *);

Q_SIGNALS:
    void triggered(QAction *);

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void actionEvent(QActionEvent *) Q_DECL_OVERRIDE;

    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *) Q_DECL_OVERRIDE;

    bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;
    void nextCheckState() Q_DECL_OVERRIDE;
    void initStyleOption(QStyleOptionToolButton *option) const;

private:
    Q_DISABLE_COPY(QToolButton)
    Q_DECLARE_PRIVATE(QToolButton)
#ifndef QT_NO_MENU
    Q_PRIVATE_SLOT(d_func(), void _q_buttonPressed())
    Q_PRIVATE_SLOT(d_func(), void _q_buttonReleased())
    Q_PRIVATE_SLOT(d_func(), void _q_updateButtonDown())
    Q_PRIVATE_SLOT(d_func(), void _q_menuTriggered(QAction*))
#endif
    Q_PRIVATE_SLOT(d_func(), void _q_actionTriggered())

};

#endif // QT_NO_TOOLBUTTON

QT_END_NAMESPACE

#endif // QTOOLBUTTON_H
