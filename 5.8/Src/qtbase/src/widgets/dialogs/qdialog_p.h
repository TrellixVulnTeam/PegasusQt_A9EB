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

#ifndef QDIALOG_P_H
#define QDIALOG_P_H

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
#include "private/qwidget_p.h"
#include "QtCore/qeventloop.h"
#include "QtCore/qpointer.h"
#include "QtWidgets/qdialog.h"
#include "QtWidgets/qpushbutton.h"
#include <qpa/qplatformdialoghelper.h>

QT_BEGIN_NAMESPACE

class QSizeGrip;

class Q_WIDGETS_EXPORT QDialogPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QDialog)
public:

    QDialogPrivate()
        : mainDef(0), orientation(Qt::Horizontal),extension(0), doShowExtension(false),
#ifndef QT_NO_SIZEGRIP
          resizer(0),
          sizeGripEnabled(false),
#endif
          rescode(0), resetModalityTo(-1), wasModalitySet(true), eventLoop(0),
          nativeDialogInUse(false), m_platformHelper(0), m_platformHelperCreated(false)
        {}
    ~QDialogPrivate();

    QWindow *parentWindow() const;
    bool setNativeDialogVisible(bool visible);
    QVariant styleHint(QPlatformDialogHelper::StyleHint hint) const;
    void deletePlatformHelper();

    QPointer<QPushButton> mainDef;
    Qt::Orientation orientation;
    QWidget *extension;
    bool doShowExtension;
    QSize size, min, max;
#ifndef QT_NO_SIZEGRIP
    QSizeGrip *resizer;
    bool sizeGripEnabled;
#endif
    QPoint lastRMBPress;

    void setDefault(QPushButton *);
    void setMainDefault(QPushButton *);
    void hideDefault();
    void resetModalitySetByOpen();

    int rescode;
    int resetModalityTo;
    bool wasModalitySet;

    QPointer<QEventLoop> eventLoop;

    bool nativeDialogInUse;
    QPlatformDialogHelper *platformHelper() const;
    virtual bool canBeNativeDialog() const;

private:
    virtual void initHelper(QPlatformDialogHelper *) {}
    virtual void helperPrepareShow(QPlatformDialogHelper *) {}
    virtual void helperDone(QDialog::DialogCode, QPlatformDialogHelper *) {}

    mutable QPlatformDialogHelper *m_platformHelper;
    mutable bool m_platformHelperCreated;
};

QT_END_NAMESPACE

#endif // QDIALOG_P_H
