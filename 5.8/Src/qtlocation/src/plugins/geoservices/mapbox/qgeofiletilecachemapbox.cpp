/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeofiletilecachemapbox.h"
#include <QtLocation/private/qgeotilespec_p.h>
#include <QDir>

QT_BEGIN_NAMESPACE

QGeoFileTileCacheMapbox::QGeoFileTileCacheMapbox(const QList<QGeoMapType> &mapTypes, int scaleFactor, const QString &directory, QObject *parent)
    :QGeoFileTileCache(directory, parent), m_mapTypes(mapTypes)
{
    m_scaleFactor = qBound(1, scaleFactor, 2);
    for (int i=0; i < mapTypes.size(); i++)
        m_mapNameToId.insert(mapTypes[i].name(), i);
}

QGeoFileTileCacheMapbox::~QGeoFileTileCacheMapbox()
{

}

QString QGeoFileTileCacheMapbox::tileSpecToFilename(const QGeoTileSpec &spec, const QString &format, const QString &directory) const
{
    QString filename = spec.plugin();
    filename += QLatin1String("-");
    filename += m_mapTypes[spec.mapId()].name();
    filename += QLatin1String("-");
    filename += QString::number(spec.zoom());
    filename += QLatin1String("-");
    filename += QString::number(spec.x());
    filename += QLatin1String("-");
    filename += QString::number(spec.y());

    //Append version if real version number to ensure backwards compatibility and eviction of old tiles
    if (spec.version() != -1) {
        filename += QLatin1String("-");
        filename += QString::number(spec.version());
    }

    filename += QLatin1String("-@");
    filename += QString::number(m_scaleFactor);
    filename += QLatin1Char('x');

    filename += QLatin1String(".");
    filename += format;

    QDir dir = QDir(directory);

    return dir.filePath(filename);
}

QGeoTileSpec QGeoFileTileCacheMapbox::filenameToTileSpec(const QString &filename) const
{
    QStringList parts = filename.split('.');

    if (parts.length() != 3)
        return QGeoTileSpec();

    QString name = parts.at(0) + parts.at(1);
    QStringList fields = name.split('-');

    int length = fields.length();
    if (length != 6 && length != 7) {
        return QGeoTileSpec();
    } else {
        int scaleIdx = fields.last().indexOf("@");
        if (scaleIdx < 0 || fields.last().size() <= (scaleIdx + 2))
            return QGeoTileSpec();
        int scaleFactor = fields.last()[scaleIdx + 1].digitValue();
        if (scaleFactor != m_scaleFactor)
           return QGeoTileSpec();
    }

    QList<int> numbers;

    bool ok = false;
    for (int i = 2; i < length-1; ++i) { // skipping -@_X
        ok = false;
        int value = fields.at(i).toInt(&ok);
        if (!ok)
            return QGeoTileSpec();
        numbers.append(value);
    }

    //File name without version, append default
    if (numbers.length() < 4)
        numbers.append(-1);

    return QGeoTileSpec(fields.at(0),
                    m_mapNameToId[fields.at(1)],
                    numbers.at(0),
                    numbers.at(1),
                    numbers.at(2),
                    numbers.at(3));
}

QT_END_NAMESPACE
