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

#ifndef QCOLUMNVIEW_P_H
#define QCOLUMNVIEW_P_H

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

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include "qcolumnview.h"

#ifndef QT_NO_QCOLUMNVIEW

#include <private/qabstractitemview_p.h>

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qpropertyanimation.h>
#include <QtWidgets/qabstractitemdelegate.h>
#include <QtWidgets/qabstractitemview.h>
#include <QtWidgets/qitemdelegate.h>
#include <qlistview.h>
#include <qevent.h>
#include <qscrollbar.h>

QT_BEGIN_NAMESPACE

class QColumnViewPreviewColumn : public QAbstractItemView {

public:
    explicit QColumnViewPreviewColumn(QWidget *parent) : QAbstractItemView(parent), previewWidget(0) {
    }

    void setPreviewWidget(QWidget *widget) {
        previewWidget = widget;
        setMinimumWidth(previewWidget->minimumWidth());
    }

    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE{
        if (!previewWidget)
            return;
        previewWidget->resize(
                qMax(previewWidget->minimumWidth(), event->size().width()),
                previewWidget->height());
        QSize p = viewport()->size();
        QSize v = previewWidget->size();
        horizontalScrollBar()->setRange(0, v.width() - p.width());
        horizontalScrollBar()->setPageStep(p.width());
        verticalScrollBar()->setRange(0, v.height() - p.height());
        verticalScrollBar()->setPageStep(p.height());

        QAbstractScrollArea::resizeEvent(event);
    }

    void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE
    {
        if (!previewWidget)
            return;
        scrollDirtyRegion(dx, dy);
        viewport()->scroll(dx, dy);

        QAbstractItemView::scrollContentsBy(dx, dy);
    }

    QRect visualRect(const QModelIndex &) const Q_DECL_OVERRIDE
    {
        return QRect();
    }
    void scrollTo(const QModelIndex &, ScrollHint) Q_DECL_OVERRIDE
    {
    }
    QModelIndex indexAt(const QPoint &) const Q_DECL_OVERRIDE
    {
        return QModelIndex();
    }
    QModelIndex moveCursor(CursorAction, Qt::KeyboardModifiers) Q_DECL_OVERRIDE
    {
        return QModelIndex();
    }
    int horizontalOffset () const Q_DECL_OVERRIDE {
        return 0;
    }
    int verticalOffset () const Q_DECL_OVERRIDE {
        return 0;
    }
    QRegion visualRegionForSelection(const QItemSelection &) const Q_DECL_OVERRIDE
    {
        return QRegion();
    }
    bool isIndexHidden(const QModelIndex &) const Q_DECL_OVERRIDE
    {
        return false;
    }
    void setSelection(const QRect &, QItemSelectionModel::SelectionFlags) Q_DECL_OVERRIDE
    {
    }
private:
    QWidget *previewWidget;
};

class Q_AUTOTEST_EXPORT QColumnViewPrivate : public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QColumnView)

public:
    QColumnViewPrivate();
    ~QColumnViewPrivate();
    void initialize();

    QAbstractItemView *createColumn(const QModelIndex &index, bool show);

    void updateScrollbars();
    void closeColumns(const QModelIndex &parent = QModelIndex(), bool build = false);
    void doLayout();
    void setPreviewWidget(QWidget *widget);
    void checkColumnCreation(const QModelIndex &parent);


    void _q_gripMoved(int offset);
    void _q_changeCurrentColumn();
    void _q_clicked(const QModelIndex &index);
    void _q_columnsInserted(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;

    QList<QAbstractItemView*> columns;
    QVector<int> columnSizes; // used during init and corner moving
    bool showResizeGrips;
    int offset;
#ifndef QT_NO_ANIMATION
    QPropertyAnimation currentAnimation;
#endif
    QWidget *previewWidget;
    QAbstractItemView *previewColumn;
};

/*!
 * This is a delegate that will paint the triangle
 */
class QColumnViewDelegate : public QItemDelegate
{

public:
    explicit QColumnViewDelegate(QObject *parent = 0) : QItemDelegate(parent) {}
    ~QColumnViewDelegate() {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
};
#endif // QT_NO_QCOLUMNVIEW


QT_END_NAMESPACE
#endif //QCOLUMNVIEW_P_H

