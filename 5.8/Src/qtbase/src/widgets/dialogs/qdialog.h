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

#ifndef QDIALOG_H
#define QDIALOG_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qwidget.h>

QT_BEGIN_NAMESPACE


class QPushButton;
class QDialogPrivate;

class Q_WIDGETS_EXPORT QDialog : public QWidget
{
    Q_OBJECT
    friend class QPushButton;

    Q_PROPERTY(bool sizeGripEnabled READ isSizeGripEnabled WRITE setSizeGripEnabled)
    Q_PROPERTY(bool modal READ isModal WRITE setModal)

public:
    explicit QDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QDialog();

    enum DialogCode { Rejected, Accepted };

    int result() const;

    void setVisible(bool visible);

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    void setExtension(QWidget* extension);
    QWidget* extension() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setSizeGripEnabled(bool);
    bool isSizeGripEnabled() const;

    void setModal(bool modal);
    void setResult(int r);

Q_SIGNALS:
    void finished(int result);
    void accepted();
    void rejected();

public Q_SLOTS:
    virtual void open();
    virtual int exec();
    virtual void done(int);
    virtual void accept();
    virtual void reject();

    void showExtension(bool);

protected:
    QDialog(QDialogPrivate &, QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());

    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *);
#endif
    bool eventFilter(QObject *, QEvent *);
    void adjustPosition(QWidget*);
private:
    Q_DECLARE_PRIVATE(QDialog)
    Q_DISABLE_COPY(QDialog)
};

QT_END_NAMESPACE

#endif // QDIALOG_H
