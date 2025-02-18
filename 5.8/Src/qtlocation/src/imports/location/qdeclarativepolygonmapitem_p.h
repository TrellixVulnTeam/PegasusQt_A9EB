/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#ifndef QDECLARATIVEPOLYGONMAPITEM
#define QDECLARATIVEPOLYGONMAPITEM

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

#include "qdeclarativegeomapitembase_p.h"
#include "qdeclarativepolylinemapitem_p.h"
#include "qgeomapitemgeometry_p.h"

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

QT_BEGIN_NAMESPACE

class MapPolygonNode;

class QGeoMapPolygonGeometry : public QGeoMapItemGeometry
{
public:
    QGeoMapPolygonGeometry();

    inline void setAssumeSimple(bool value) { assumeSimple_ = value; }

    void updateSourcePoints(const QGeoMap &map,
                            const QList<QGeoCoordinate> &path);

    void updateScreenPoints(const QGeoMap &map);

protected:
    QPainterPath srcPath_;
    bool assumeSimple_;
};

class QDeclarativePolygonMapItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT

    Q_PROPERTY(QJSValue path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeMapLineProperties *border READ border CONSTANT)

public:
    explicit QDeclarativePolygonMapItem(QQuickItem *parent = 0);
    ~QDeclarativePolygonMapItem();

    virtual void setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map) Q_DECL_OVERRIDE;
    //from QuickItem
    virtual QSGNode *updateMapItemPaintNode(QSGNode *, UpdatePaintNodeData *) Q_DECL_OVERRIDE;

    Q_INVOKABLE void addCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void removeCoordinate(const QGeoCoordinate &coordinate);

    QJSValue path() const;
    void setPath(const QJSValue &value);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeMapLineProperties *border();

    bool contains(const QPointF &point) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void pathChanged();
    void colorChanged(const QColor &color);

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) Q_DECL_OVERRIDE;
    void updatePolish() Q_DECL_OVERRIDE;

protected Q_SLOTS:
    void handleBorderUpdated();
    virtual void afterViewportChanged(const QGeoMapViewportChangeEvent &event) Q_DECL_OVERRIDE;

private:
    void pathPropertyChanged();

    QDeclarativeMapLineProperties border_;
    QList<QGeoCoordinate> path_;
    QGeoCoordinate geoLeftBound_;
    QColor color_;
    bool dirtyMaterial_;
    QGeoMapPolygonGeometry geometry_;
    QGeoMapPolylineGeometry borderGeometry_;
    bool updatingGeometry_;
    // for the left bound calculation
    QVector<double> deltaXs_; // longitude deltas from path_[0]
    double minX_;             // minimum value inside deltaXs_
};

//////////////////////////////////////////////////////////////////////

class MapPolygonNode : public QSGGeometryNode
{

public:
    MapPolygonNode();
    ~MapPolygonNode();

    void update(const QColor &fillColor, const QColor &borderColor,
                const QGeoMapItemGeometry *fillShape,
                const QGeoMapItemGeometry *borderShape);

    bool isSubtreeBlocked() const;

private:
    QSGFlatColorMaterial fill_material_;
    MapPolylineNode *border_;
    QSGGeometry geometry_;
    bool blocked_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePolygonMapItem)

#endif /* QDECLARATIVEPOLYGONMAPITEM_H_ */
