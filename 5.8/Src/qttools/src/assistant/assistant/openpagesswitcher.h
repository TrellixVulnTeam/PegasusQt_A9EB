/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Assistant module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef OPENPAGESSWITCHER_H
#define OPENPAGESSWITCHER_H

#include <QtWidgets/QFrame>

QT_BEGIN_NAMESPACE

class OpenPagesModel;
class OpenPagesWidget;
class QModelIndex;

class OpenPagesSwitcher : public QFrame
{
    Q_OBJECT

public:
    OpenPagesSwitcher(OpenPagesModel *model);
    ~OpenPagesSwitcher();

    void gotoNextPage();
    void gotoPreviousPage();

    void selectAndHide();
    void selectCurrentPage();

    void setVisible(bool visible);
    void focusInEvent(QFocusEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

signals:
    void closePage(const QModelIndex &index);
    void setCurrentPage(const QModelIndex &index);

private:
    void selectPageUpDown(int summand);

private:
    OpenPagesModel *m_openPagesModel;
    OpenPagesWidget *m_openPagesWidget;
};

QT_END_NAMESPACE

#endif  // OPENPAGESSWITCHER_H
