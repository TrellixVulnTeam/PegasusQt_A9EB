/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef PROPERTY_H
#define PROPERTY_H

#include "library/proitems.h"

#include <qglobal.h>
#include <qstring.h>
#include <qhash.h>

QT_BEGIN_NAMESPACE

class QSettings;

class QMakeProperty
{
    QSettings *settings;
    void initSettings();

    QHash<ProKey, ProString> m_values;

public:
    QMakeProperty();
    ~QMakeProperty();

    bool hasValue(const ProKey &);
    ProString value(const ProKey &);

    void setValue(QString, const QString &);
    void remove(const QString &);

    bool exec();
};

QT_END_NAMESPACE

#endif // PROPERTY_H
