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

#ifndef QTOKENIZER_P_H
#define QTOKENIZER_P_H

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

#include "qtoken_p.h"
#include "qreader_p.h"
#include "qtokenstream_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QChar>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

class Q_CLUCENE_EXPORT QCLuceneTokenizer : public QCLuceneTokenStream
{
public:
    QCLuceneTokenizer(const QCLuceneReader &reader);
    virtual ~QCLuceneTokenizer();

    void close();
    bool next(QCLuceneToken &token);

protected:
    friend class QCLuceneStandardTokenizer;

private:
    QCLuceneTokenizer();
    QCLuceneReader reader;
};

class Q_CLUCENE_EXPORT QCLuceneStandardTokenizer : public QCLuceneTokenizer
{
public:
    QCLuceneStandardTokenizer(const QCLuceneReader &reader);
    ~QCLuceneStandardTokenizer();

    bool readApostrophe(const QString &string, QCLuceneToken &token);
    bool readAt(const QString &string, QCLuceneToken &token);
    bool readCompany(const QString &string, QCLuceneToken &token);
};

class QCLuceneCharTokenizer : public QCLuceneTokenizer
{

};

class QCLuceneLetterTokenizer : public QCLuceneCharTokenizer
{

};

class QCLuceneLowerCaseTokenizer : public QCLuceneLetterTokenizer
{

};

class QCLuceneWhitespaceTokenizer : public QCLuceneCharTokenizer
{

};

class QCLuceneKeywordTokenizer : public QCLuceneTokenizer
{

};

QT_END_NAMESPACE

#endif  // QTOKENIZER_P_H
