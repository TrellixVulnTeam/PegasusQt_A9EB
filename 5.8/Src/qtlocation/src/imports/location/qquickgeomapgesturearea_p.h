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

#ifndef QQUICKGEOMAPGESTUREAREA_P_H
#define QQUICKGEOMAPGESTUREAREA_P_H

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

#include <QtQuick/QQuickItem>
#include <QTouchEvent>
#include <QDebug>
#include <QElapsedTimer>
#include "qgeocoordinate.h"
#include "qgeomap_p.h"
#include "qquickgeocoordinateanimation_p.h"

QT_BEGIN_NAMESPACE

class QGraphicsSceneMouseEvent;
class QDeclarativeGeoMap;
class QTouchEvent;
class QWheelEvent;
class QGeoMap;

class QGeoMapPinchEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPointF center READ center)
    Q_PROPERTY(qreal angle READ angle)
    Q_PROPERTY(QPointF point1 READ point1)
    Q_PROPERTY(QPointF point2 READ point2)
    Q_PROPERTY(int pointCount READ pointCount)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)

public:
    QGeoMapPinchEvent(const QPointF &center, qreal angle,
                                 const QPointF &point1, const QPointF &point2,
                                 int pointCount = 0, bool accepted = true)
        : QObject(), m_center(center), m_angle(angle),
          m_point1(point1), m_point2(point2),
        m_pointCount(pointCount), m_accepted(accepted) {}
    QGeoMapPinchEvent()
        : QObject(),
          m_angle(0.0),
          m_pointCount(0),
          m_accepted(true) {}

    QPointF center() const { return m_center; }
    void setCenter(const QPointF &center) { m_center = center; }
    qreal angle() const { return m_angle; }
    void setAngle(qreal angle) { m_angle = angle; }
    QPointF point1() const { return m_point1; }
    void setPoint1(const QPointF &p) { m_point1 = p; }
    QPointF point2() const { return m_point2; }
    void setPoint2(const QPointF &p) { m_point2 = p; }
    int pointCount() const { return m_pointCount; }
    void setPointCount(int count) { m_pointCount = count; }
    bool accepted() const { return m_accepted; }
    void setAccepted(bool a) { m_accepted = a; }

private:
    QPointF m_center;
    qreal m_angle;
    QPointF m_point1;
    QPointF m_point2;
    int m_pointCount;
    bool m_accepted;
};

class QQuickGeoMapGestureArea: public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(GeoMapGesture)
    Q_FLAGS(AcceptedGestures)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool pinchActive READ isPinchActive NOTIFY pinchActiveChanged)
    Q_PROPERTY(bool panActive READ isPanActive NOTIFY panActiveChanged)
    Q_PROPERTY(AcceptedGestures acceptedGestures READ acceptedGestures WRITE setAcceptedGestures NOTIFY acceptedGesturesChanged)
    Q_PROPERTY(qreal maximumZoomLevelChange READ maximumZoomLevelChange WRITE setMaximumZoomLevelChange NOTIFY maximumZoomLevelChangeChanged)
    Q_PROPERTY(qreal flickDeceleration READ flickDeceleration WRITE setFlickDeceleration NOTIFY flickDecelerationChanged)
    Q_PROPERTY(bool preventStealing READ preventStealing WRITE setPreventStealing NOTIFY preventStealingChanged REVISION 1)

public:
    QQuickGeoMapGestureArea(QDeclarativeGeoMap *map);
    ~QQuickGeoMapGestureArea();

    enum GeoMapGesture {
        NoGesture = 0x0000,
        PinchGesture = 0x0001,
        PanGesture = 0x0002,
        FlickGesture = 0x004
    };

    Q_DECLARE_FLAGS(AcceptedGestures, GeoMapGesture)

    AcceptedGestures acceptedGestures() const;
    void setAcceptedGestures(AcceptedGestures acceptedGestures);

    bool isPinchActive() const;
    bool isPanActive() const;
    bool isActive() const;

    bool enabled() const;
    void setEnabled(bool enabled);

    qreal maximumZoomLevelChange() const;
    void setMaximumZoomLevelChange(qreal maxChange);

    qreal flickDeceleration() const;
    void setFlickDeceleration(qreal deceleration);

    void handleTouchEvent(QTouchEvent *event);
    void handleWheelEvent(QWheelEvent *event);
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseUngrabEvent();
    void handleTouchUngrabEvent();

    void setMinimumZoomLevel(qreal min);
    qreal minimumZoomLevel() const;

    void setMaximumZoomLevel(qreal max);
    qreal maximumZoomLevel() const;

    void setMap(QGeoMap *map);

    bool preventStealing() const;
    void setPreventStealing(bool prevent);

Q_SIGNALS:
    void panActiveChanged();
    void pinchActiveChanged();
    void enabledChanged();
    void maximumZoomLevelChangeChanged();
    void acceptedGesturesChanged();
    void flickDecelerationChanged();
    void pinchStarted(QGeoMapPinchEvent *pinch);
    void pinchUpdated(QGeoMapPinchEvent *pinch);
    void pinchFinished(QGeoMapPinchEvent *pinch);
    void panStarted();
    void panFinished();
    void flickStarted();
    void flickFinished();
    void preventStealingChanged();
private:
    void update();

    // Create general data relating to the touch points
    void touchPointStateMachine();
    void startOneTouchPoint();
    void updateOneTouchPoint();
    void startTwoTouchPoints();
    void updateTwoTouchPoints();

    // All pinch related code, which encompasses zoom
    void pinchStateMachine();
    bool canStartPinch();
    void startPinch();
    void updatePinch();
    void endPinch();

    // Pan related code (regardles of number of touch points),
    // includes the flick based panning after letting go
    void panStateMachine();
    bool canStartPan();
    void updatePan();
    bool tryStartFlick();
    void startFlick(int dx, int dy, int timeMs = 0);
    void stopFlick();

    bool pinchEnabled() const;
    void setPinchEnabled(bool enabled);
    bool panEnabled() const;
    void setPanEnabled(bool enabled);
    bool flickEnabled() const;
    void setFlickEnabled(bool enabled);

private Q_SLOTS:
    void handleFlickAnimationStopped();


private:
    void stopPan();
    void clearTouchData();
    void updateVelocityList(const QPointF &pos);

private:
    QGeoMap *m_map;
    QDeclarativeGeoMap *m_declarativeMap;
    bool m_enabled;

    struct Pinch
    {
        Pinch() : m_enabled(true), m_startDist(0), m_lastAngle(0.0) {}

        QGeoMapPinchEvent m_event;
        bool m_enabled;
        struct Zoom
        {
            Zoom() : m_minimum(-1.0), m_maximum(20.0), m_start(0.0), m_previous(0.0),
                     maximumChange(4.0) {}
            qreal m_minimum;
            qreal m_maximum;
            qreal m_start;
            qreal m_previous;
            qreal maximumChange;
        } m_zoom;

        QPointF m_lastPoint1;
        QPointF m_lastPoint2;
        qreal m_startDist;
        qreal m_lastAngle;
     } m_pinch;

    AcceptedGestures m_acceptedGestures;

    struct Pan
    {
        qreal m_maxVelocity;
        qreal m_deceleration;
        QQuickGeoCoordinateAnimation *m_animation;
        bool m_enabled;
    } m_flick;


    // these are calculated regardless of gesture or number of touch points
    qreal m_velocityX;
    qreal m_velocityY;
    QElapsedTimer m_lastPosTime;
    QPointF m_lastPos;
    QList<QTouchEvent::TouchPoint> m_allPoints;
    QList<QTouchEvent::TouchPoint> m_touchPoints;
    QScopedPointer<QTouchEvent::TouchPoint> m_mousePoint;
    QPointF m_sceneStartPoint1;

    // only set when two points in contact
    QPointF m_sceneStartPoint2;
    QGeoCoordinate m_startCoord;
    QGeoCoordinate m_touchCenterCoord;
    qreal m_twoTouchAngle;
    qreal m_distanceBetweenTouchPoints;
    QPointF m_sceneCenter;
    bool m_preventStealing;
    bool m_panEnabled;

    // prototype state machine...
    enum TouchPointState
    {
        touchPoints0,
        touchPoints1,
        touchPoints2
    } m_touchPointState;

    enum PinchState
    {
        pinchInactive,
        pinchInactiveTwoPoints,
        pinchActive
    } m_pinchState;

    enum FlickState
    {
        flickInactive,
        panActive,
        flickActive
    } m_flickState;
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QQuickGeoMapGestureArea)

#endif // QQUICKGEOMAPGESTUREAREA_P_H
