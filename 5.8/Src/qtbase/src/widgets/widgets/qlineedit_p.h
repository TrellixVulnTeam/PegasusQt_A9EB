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

#ifndef QLINEEDIT_P_H
#define QLINEEDIT_P_H

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

#ifndef QT_NO_LINEEDIT
#include "private/qwidget_p.h"
#include "QtWidgets/qlineedit.h"
#include "QtWidgets/qtoolbutton.h"
#include "QtGui/qtextlayout.h"
#include "QtGui/qicon.h"
#include "QtWidgets/qstyleoption.h"
#include "QtCore/qbasictimer.h"
#include "QtWidgets/qcompleter.h"
#include "QtCore/qpointer.h"
#include "QtCore/qmimedata.h"

#include "private/qwidgetlinecontrol_p.h"

#include <algorithm>

QT_BEGIN_NAMESPACE

class QLineEditPrivate;

// QLineEditIconButton: This is a simple helper class that represents clickable icons that fade in with text

class Q_AUTOTEST_EXPORT QLineEditIconButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
    explicit QLineEditIconButton(QWidget *parent =  0);

    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal value);
#ifndef QT_NO_ANIMATION
    void animateShow(bool visible) { startOpacityAnimation(visible ? 1.0 : 0.0); }
#endif

protected:
    void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private slots:
    void updateCursor();

private:
#ifndef QT_NO_ANIMATION
    void startOpacityAnimation(qreal endValue);
#endif
    QLineEditPrivate *lineEditPrivate() const;

    qreal m_opacity;
};

class Q_AUTOTEST_EXPORT QLineEditPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QLineEdit)
public:
    enum SideWidgetFlag {
        SideWidgetFadeInWithText = 0x1,
        SideWidgetCreatedByWidgetAction = 0x2,
        SideWidgetClearButton = 0x4
    };

    struct SideWidgetEntry {
        explicit SideWidgetEntry(QWidget *w = 0, QAction *a = 0, int _flags = 0) : widget(w), action(a), flags(_flags) {}

        QWidget *widget;
        QAction *action;
        int flags;
    };
    typedef std::vector<SideWidgetEntry> SideWidgetEntryList;

    struct SideWidgetParameters {
        int iconSize;
        int widgetWidth;
        int widgetHeight;
        int margin;
    };

    QLineEditPrivate()
        : control(0), frame(1), contextMenuEnabled(1), cursorVisible(0),
        dragEnabled(0), clickCausedFocus(0), hscroll(0), vscroll(0),
        alignment(Qt::AlignLeading | Qt::AlignVCenter),
        leftTextMargin(0), topTextMargin(0), rightTextMargin(0), bottomTextMargin(0),
        lastTextSize(0)
    {
    }

    ~QLineEditPrivate()
    {
    }

    QWidgetLineControl *control;

#ifndef QT_NO_CONTEXTMENU
    QPointer<QAction> selectAllAction;
#endif
    void init(const QString&);

    QRect adjustedControlRect(const QRect &) const;

    int xToPos(int x, QTextLine::CursorPosition = QTextLine::CursorBetweenCharacters) const;
    bool inSelection(int x) const;
    QRect cursorRect() const;
    void setCursorVisible(bool visible);

    void updatePasswordEchoEditing(bool);

    void resetInputMethod();

    inline bool shouldEnableInputMethod() const
    {
        return !control->isReadOnly();
    }
    inline bool shouldShowPlaceholderText() const
    {
        return control->text().isEmpty() && control->preeditAreaText().isEmpty()
                && !((alignment & Qt::AlignHCenter) && q_func()->hasFocus());
    }

    static inline QLineEditPrivate *get(QLineEdit *lineEdit) {
        return lineEdit->d_func();
    }

    QPoint tripleClick;
    QBasicTimer tripleClickTimer;
    uint frame : 1;
    uint contextMenuEnabled : 1;
    uint cursorVisible : 1;
    uint dragEnabled : 1;
    uint clickCausedFocus : 1;
    int hscroll;
    int vscroll;
    uint alignment;
    static const int verticalMargin;
    static const int horizontalMargin;

    bool sendMouseEventToInputContext(QMouseEvent *e);

    QRect adjustedContentsRect() const;

    void _q_handleWindowActivate();
    void _q_textEdited(const QString &);
    void _q_cursorPositionChanged(int, int);
#ifdef QT_KEYPAD_NAVIGATION
    void _q_editFocusChange(bool);
#endif
    void _q_selectionChanged();
    void _q_updateNeeded(const QRect &);
#ifndef QT_NO_COMPLETER
    void _q_completionHighlighted(const QString &);
#endif
    QPoint mousePressPos;
#ifndef QT_NO_DRAGANDDROP
    QBasicTimer dndTimer;
    void drag();
#endif
    void _q_textChanged(const QString &);
    void _q_clearButtonClicked();

    int leftTextMargin; // use effectiveLeftTextMargin() in case of icon.
    int topTextMargin;
    int rightTextMargin; // use effectiveRightTextMargin() in case of icon.
    int bottomTextMargin;

    QString placeholderText;

    QWidget *addAction(QAction *newAction, QAction *before, QLineEdit::ActionPosition, int flags = 0);
    void removeAction(QAction *action);
    SideWidgetParameters sideWidgetParameters() const;
    QIcon clearButtonIcon() const;
    void setClearButtonEnabled(bool enabled);
    void positionSideWidgets();
    inline bool hasSideWidgets() const { return !leadingSideWidgets.empty() || !trailingSideWidgets.empty(); }
    inline const SideWidgetEntryList &leftSideWidgetList() const
        { return q_func()->layoutDirection() == Qt::LeftToRight ? leadingSideWidgets : trailingSideWidgets; }
    inline const SideWidgetEntryList &rightSideWidgetList() const
        { return q_func()->layoutDirection() == Qt::LeftToRight ? trailingSideWidgets : leadingSideWidgets; }

    int effectiveLeftTextMargin() const;
    int effectiveRightTextMargin() const;

private:
    typedef QPair<QLineEdit::ActionPosition, int> PositionIndexPair;

    PositionIndexPair findSideWidget(const QAction *a) const;

    SideWidgetEntryList leadingSideWidgets;
    SideWidgetEntryList trailingSideWidgets;
    int lastTextSize;
};
Q_DECLARE_TYPEINFO(QLineEditPrivate::SideWidgetEntry, Q_PRIMITIVE_TYPE);

#endif // QT_NO_LINEEDIT

QT_END_NAMESPACE

#endif // QLINEEDIT_P_H
