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

#ifndef QGRAPHICSEFFECT_H
#define QGRAPHICSEFFECT_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

#ifndef QT_NO_GRAPHICSEFFECT
QT_BEGIN_NAMESPACE


class QGraphicsItem;
class QStyleOption;
class QPainter;
class QPixmap;

class QGraphicsEffectSource;

class QGraphicsEffectPrivate;
class Q_WIDGETS_EXPORT QGraphicsEffect : public QObject
{
    Q_OBJECT
    Q_FLAGS(ChangeFlags)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:
    enum ChangeFlag {
        SourceAttached = 0x1,
        SourceDetached = 0x2,
        SourceBoundingRectChanged = 0x4,
        SourceInvalidated = 0x8
    };
    Q_DECLARE_FLAGS(ChangeFlags, ChangeFlag)

    enum PixmapPadMode {
        NoPad,
        PadToTransparentBorder,
        PadToEffectiveBoundingRect
    };

    QGraphicsEffect(QObject *parent = Q_NULLPTR);
    virtual ~QGraphicsEffect();

    virtual QRectF boundingRectFor(const QRectF &sourceRect) const;
    QRectF boundingRect() const;

    bool isEnabled() const;

public Q_SLOTS:
    void setEnabled(bool enable);
    void update();

Q_SIGNALS:
    void enabledChanged(bool enabled);

protected:
    QGraphicsEffect(QGraphicsEffectPrivate &d, QObject *parent = Q_NULLPTR);
    virtual void draw(QPainter *painter) = 0;
    virtual void sourceChanged(ChangeFlags flags);
    void updateBoundingRect();

    bool sourceIsPixmap() const;
    QRectF sourceBoundingRect(Qt::CoordinateSystem system = Qt::LogicalCoordinates) const;
    void drawSource(QPainter *painter);
    QPixmap sourcePixmap(Qt::CoordinateSystem system = Qt::LogicalCoordinates,
                         QPoint *offset = Q_NULLPTR,
                         PixmapPadMode mode = PadToEffectiveBoundingRect) const;

private:
    Q_DECLARE_PRIVATE(QGraphicsEffect)
    Q_DISABLE_COPY(QGraphicsEffect)
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    friend class QGraphicsScenePrivate;
    friend class QWidget;
    friend class QWidgetPrivate;

public:
    QGraphicsEffectSource *source() const; // internal

};
Q_DECLARE_OPERATORS_FOR_FLAGS(QGraphicsEffect::ChangeFlags)

class QGraphicsColorizeEffectPrivate;
class Q_WIDGETS_EXPORT QGraphicsColorizeEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
public:
    QGraphicsColorizeEffect(QObject *parent = Q_NULLPTR);
    ~QGraphicsColorizeEffect();

    QColor color() const;
    qreal strength() const;

public Q_SLOTS:
    void setColor(const QColor &c);
    void setStrength(qreal strength);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void strengthChanged(qreal strength);

protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QGraphicsColorizeEffect)
    Q_DISABLE_COPY(QGraphicsColorizeEffect)
};

class QGraphicsBlurEffectPrivate;
class Q_WIDGETS_EXPORT QGraphicsBlurEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_FLAGS(BlurHint BlurHints)
    Q_PROPERTY(qreal blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(BlurHints blurHints READ blurHints WRITE setBlurHints NOTIFY blurHintsChanged)
public:
    enum BlurHint {
        PerformanceHint = 0x00,
        QualityHint = 0x01,
        AnimationHint = 0x02
    };
    Q_DECLARE_FLAGS(BlurHints, BlurHint)

    QGraphicsBlurEffect(QObject *parent = Q_NULLPTR);
    ~QGraphicsBlurEffect();

    QRectF boundingRectFor(const QRectF &rect) const Q_DECL_OVERRIDE;
    qreal blurRadius() const;
    BlurHints blurHints() const;

public Q_SLOTS:
    void setBlurRadius(qreal blurRadius);
    void setBlurHints(BlurHints hints);

Q_SIGNALS:
    void blurRadiusChanged(qreal blurRadius);
    void blurHintsChanged(BlurHints hints);

protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QGraphicsBlurEffect)
    Q_DISABLE_COPY(QGraphicsBlurEffect)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGraphicsBlurEffect::BlurHints)

class QGraphicsDropShadowEffectPrivate;
class Q_WIDGETS_EXPORT QGraphicsDropShadowEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(QPointF offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    QGraphicsDropShadowEffect(QObject *parent = Q_NULLPTR);
    ~QGraphicsDropShadowEffect();

    QRectF boundingRectFor(const QRectF &rect) const Q_DECL_OVERRIDE;
    QPointF offset() const;

    inline qreal xOffset() const
    { return offset().x(); }

    inline qreal yOffset() const
    { return offset().y(); }

    qreal blurRadius() const;
    QColor color() const;

public Q_SLOTS:
    void setOffset(const QPointF &ofs);

    inline void setOffset(qreal dx, qreal dy)
    { setOffset(QPointF(dx, dy)); }

    inline void setOffset(qreal d)
    { setOffset(QPointF(d, d)); }

    inline void setXOffset(qreal dx)
    { setOffset(QPointF(dx, yOffset())); }

    inline void setYOffset(qreal dy)
    { setOffset(QPointF(xOffset(), dy)); }

    void setBlurRadius(qreal blurRadius);
    void setColor(const QColor &color);

Q_SIGNALS:
    void offsetChanged(const QPointF &offset);
    void blurRadiusChanged(qreal blurRadius);
    void colorChanged(const QColor &color);

protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QGraphicsDropShadowEffect)
    Q_DISABLE_COPY(QGraphicsDropShadowEffect)
};

class QGraphicsOpacityEffectPrivate;
class Q_WIDGETS_EXPORT QGraphicsOpacityEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(QBrush opacityMask READ opacityMask WRITE setOpacityMask NOTIFY opacityMaskChanged)
public:
    QGraphicsOpacityEffect(QObject *parent = Q_NULLPTR);
    ~QGraphicsOpacityEffect();

    qreal opacity() const;
    QBrush opacityMask() const;

public Q_SLOTS:
    void setOpacity(qreal opacity);
    void setOpacityMask(const QBrush &mask);

Q_SIGNALS:
    void opacityChanged(qreal opacity);
    void opacityMaskChanged(const QBrush &mask);

protected:
    void draw(QPainter *painter) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QGraphicsOpacityEffect)
    Q_DISABLE_COPY(QGraphicsOpacityEffect)
};

QT_END_NAMESPACE

#endif //QT_NO_GRAPHICSEFFECT

#endif // QGRAPHICSEFFECT_H

