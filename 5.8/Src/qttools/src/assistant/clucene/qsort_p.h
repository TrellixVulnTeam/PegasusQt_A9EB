/****************************************************************************
**
** Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team.
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtTools module of the Qt Toolkit.
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

#ifndef QSORT_P_H
#define QSORT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#include "qclucene_global_p.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(search)
    class Sort;
CL_NS_END
CL_NS_USE(search)

QT_BEGIN_NAMESPACE

class QCLuceneHits;
class QCLuceneField;

class Q_CLUCENE_EXPORT QCLuceneSortPrivate : public QSharedData
{
public:
    QCLuceneSortPrivate();
    QCLuceneSortPrivate (const QCLuceneSortPrivate &other);

    ~QCLuceneSortPrivate();

    Sort *sort;
    bool deleteCLuceneSort;

private:
    QCLuceneSortPrivate &operator=(const QCLuceneSortPrivate &other);
};

class Q_CLUCENE_EXPORT QCLuceneSort
{
public:
    QCLuceneSort();
    explicit QCLuceneSort(const QStringList &fieldNames);
    explicit QCLuceneSort(const QString &field, bool reverse = false);

    virtual ~QCLuceneSort();

    QString toString() const;
    void setSort(const QStringList &fieldNames);
    void setSort(const QString &field, bool reverse = false);

protected:
    friend class QCLuceneHits;
    QSharedDataPointer<QCLuceneSortPrivate> d;
};

QT_END_NAMESPACE

#endif  // QSORT_P_H
