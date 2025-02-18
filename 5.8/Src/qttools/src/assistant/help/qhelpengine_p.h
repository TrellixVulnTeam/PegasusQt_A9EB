/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef QHELPENGINE_P_H
#define QHELPENGINE_P_H

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

#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QSqlQuery;

class QHelpEngineCore;
class QHelpDBReader;
class QHelpContentModel;
class QHelpContentWidget;
class QHelpIndexModel;
class QHelpIndexWidget;
class QHelpSearchEngine;
class QHelpCollectionHandler;

class QHelpEngineCorePrivate : public QObject
{
    Q_OBJECT

public:
    QHelpEngineCorePrivate();
    virtual ~QHelpEngineCorePrivate();

    virtual void init(const QString &collectionFile,
        QHelpEngineCore *helpEngineCore);

    void clearMaps();
    bool setup();

    QMap<QString, QHelpDBReader*> readerMap;
    QMap<QString, QHelpDBReader*> fileNameReaderMap;
    QMultiMap<QString, QHelpDBReader*> virtualFolderMap;
    QStringList orderedFileNameList;

    QHelpCollectionHandler *collectionHandler;
    QString currentFilter;
    QString error;
    bool needsSetup;
    bool autoSaveFilter;

protected:
    QHelpEngineCore *q;

private slots:
    void errorReceived(const QString &msg);
};


class QHelpEnginePrivate : public QHelpEngineCorePrivate
{
    Q_OBJECT

public:
    QHelpEnginePrivate();
    ~QHelpEnginePrivate();

    void init(const QString &collectionFile,
        QHelpEngineCore *helpEngineCore);

    QHelpContentModel *contentModel;
    QHelpContentWidget *contentWidget;

    QHelpIndexModel *indexModel;
    QHelpIndexWidget *indexWidget;

    QHelpSearchEngine *searchEngine;

    void stopDataCollection();

    friend class QHelpContentProvider;
    friend class QHelpContentModel;
    friend class QHelpIndexProvider;
    friend class QHelpIndexModel;

public slots:
    void setContentsWidgetBusy();
    void unsetContentsWidgetBusy();
    void setIndexWidgetBusy();
    void unsetIndexWidgetBusy();

private slots:
    void applyCurrentFilter();
};

QT_END_NAMESPACE

#endif
