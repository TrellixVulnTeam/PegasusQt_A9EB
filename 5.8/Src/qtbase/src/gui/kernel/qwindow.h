/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QWINDOW_H
#define QWINDOW_H

#include <QtGui/qtguiglobal.h>
#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QMargins>
#include <QtCore/QRect>

#include <QtCore/qnamespace.h>

#include <QtGui/qsurface.h>
#include <QtGui/qsurfaceformat.h>
#include <QtGui/qwindowdefs.h>

#include <QtGui/qicon.h>

#ifndef QT_NO_CURSOR
#include <QtGui/qcursor.h>
#endif

QT_BEGIN_NAMESPACE


class QWindowPrivate;

class QExposeEvent;
class QFocusEvent;
class QMoveEvent;
class QResizeEvent;
class QShowEvent;
class QHideEvent;
class QKeyEvent;
class QMouseEvent;
#ifndef QT_NO_WHEELEVENT
class QWheelEvent;
#endif
class QTouchEvent;
#ifndef QT_NO_TABLETEVENT
class QTabletEvent;
#endif

class QPlatformSurface;
class QPlatformWindow;
class QBackingStore;
class QScreen;
class QAccessibleInterface;
class QWindowContainer;
#ifndef QT_NO_DEBUG_STREAM
class QDebug;
#endif

class Q_GUI_EXPORT QWindow : public QObject, public QSurface
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWindow)

    // All properties which are declared here are inherited by QQuickWindow and therefore available in QML.
    // So please think carefully about what it does to the QML namespace if you add any new ones,
    // particularly the possible meanings these names might have in any specializations of Window.
    // For example "state" (meaning windowState) is not a good property to declare, because it has
    // a different meaning in QQuickItem, and users will tend to assume it is the same for Window.

    // Any new properties which you add here MUST be versioned and MUST be documented both as
    // C++ properties in qwindow.cpp AND as QML properties in qquickwindow.cpp.
    // http://qt-project.org/doc/qt-5.0/qtqml/qtqml-cppintegration-definetypes.html#type-revisions-and-versions

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY windowTitleChanged)
    Q_PROPERTY(Qt::WindowModality modality READ modality WRITE setModality NOTIFY modalityChanged)
    Q_PROPERTY(Qt::WindowFlags flags READ flags WRITE setFlags)
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)
    Q_PROPERTY(int minimumHeight READ minimumHeight WRITE setMinimumHeight NOTIFY minimumHeightChanged)
    Q_PROPERTY(int maximumWidth READ maximumWidth WRITE setMaximumWidth NOTIFY maximumWidthChanged)
    Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight NOTIFY maximumHeightChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged REVISION 1)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged REVISION 1)
    Q_PROPERTY(Qt::ScreenOrientation contentOrientation READ contentOrientation WRITE reportContentOrientationChange NOTIFY contentOrientationChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged REVISION 1)

public:
    enum Visibility {
        Hidden = 0,
        AutomaticVisibility,
        Windowed,
        Minimized,
        Maximized,
        FullScreen
    };
    Q_ENUM(Visibility)

    explicit QWindow(QScreen *screen = Q_NULLPTR);
    explicit QWindow(QWindow *parent);
    virtual ~QWindow();

    void setSurfaceType(SurfaceType surfaceType);
    SurfaceType surfaceType() const Q_DECL_OVERRIDE;

    bool isVisible() const;

    Visibility visibility() const;
    void setVisibility(Visibility v);

    void create();

    WId winId() const;

    QWindow *parent() const;
    void setParent(QWindow *parent);

    bool isTopLevel() const;

    bool isModal() const;
    Qt::WindowModality modality() const;
    void setModality(Qt::WindowModality modality);

    void setFormat(const QSurfaceFormat &format);
    QSurfaceFormat format() const Q_DECL_OVERRIDE;
    QSurfaceFormat requestedFormat() const;

    void setFlags(Qt::WindowFlags flags);
    Qt::WindowFlags flags() const;
    Qt::WindowType type() const;

    QString title() const;

    void setOpacity(qreal level);
    qreal opacity() const;

    void setMask(const QRegion &region);
    QRegion mask() const;

    bool isActive() const;

    void reportContentOrientationChange(Qt::ScreenOrientation orientation);
    Qt::ScreenOrientation contentOrientation() const;

    qreal devicePixelRatio() const;

    Qt::WindowState windowState() const;
    void setWindowState(Qt::WindowState state);

    void setTransientParent(QWindow *parent);
    QWindow *transientParent() const;

    enum AncestorMode {
        ExcludeTransients,
        IncludeTransients
    };

    bool isAncestorOf(const QWindow *child, AncestorMode mode = IncludeTransients) const;

    bool isExposed() const;

    inline int minimumWidth() const { return minimumSize().width(); }
    inline int minimumHeight() const { return minimumSize().height(); }
    inline int maximumWidth() const { return maximumSize().width(); }
    inline int maximumHeight() const { return maximumSize().height(); }

    QSize minimumSize() const;
    QSize maximumSize() const;
    QSize baseSize() const;
    QSize sizeIncrement() const;

    void setMinimumSize(const QSize &size);
    void setMaximumSize(const QSize &size);
    void setBaseSize(const QSize &size);
    void setSizeIncrement(const QSize &size);

    void setGeometry(int posx, int posy, int w, int h);
    void setGeometry(const QRect &rect);
    QRect geometry() const;

    QMargins frameMargins() const;
    QRect frameGeometry() const;

    QPoint framePosition() const;
    void setFramePosition(const QPoint &point);

    inline int width() const { return geometry().width(); }
    inline int height() const { return geometry().height(); }
    inline int x() const { return geometry().x(); }
    inline int y() const { return geometry().y(); }

    QSize size() const Q_DECL_OVERRIDE { return geometry().size(); }
    inline QPoint position() const { return geometry().topLeft(); }

    void setPosition(const QPoint &pt);
    void setPosition(int posx, int posy);

    void resize(const QSize &newSize);
    void resize(int w, int h);

    void setFilePath(const QString &filePath);
    QString filePath() const;

    void setIcon(const QIcon &icon);
    QIcon icon() const;

    void destroy();

    QPlatformWindow *handle() const;

    bool setKeyboardGrabEnabled(bool grab);
    bool setMouseGrabEnabled(bool grab);

    QScreen *screen() const;
    void setScreen(QScreen *screen);

    virtual QAccessibleInterface *accessibleRoot() const;
    virtual QObject *focusObject() const;

    QPoint mapToGlobal(const QPoint &pos) const;
    QPoint mapFromGlobal(const QPoint &pos) const;

#ifndef QT_NO_CURSOR
    QCursor cursor() const;
    void setCursor(const QCursor &);
    void unsetCursor();
#endif

    static QWindow *fromWinId(WId id);

public Q_SLOTS:
    Q_REVISION(1) void requestActivate();

    void setVisible(bool visible);

    void show();
    void hide();

    void showMinimized();
    void showMaximized();
    void showFullScreen();
    void showNormal();

    bool close();
    void raise();
    void lower();

    void setTitle(const QString &);

    void setX(int arg);
    void setY(int arg);
    void setWidth(int arg);
    void setHeight(int arg);

    void setMinimumWidth(int w);
    void setMinimumHeight(int h);
    void setMaximumWidth(int w);
    void setMaximumHeight(int h);

    Q_REVISION(1) void alert(int msec);

    Q_REVISION(3) void requestUpdate();

Q_SIGNALS:
    void screenChanged(QScreen *screen);
    void modalityChanged(Qt::WindowModality modality);
    void windowStateChanged(Qt::WindowState windowState);
    Q_REVISION(2) void windowTitleChanged(const QString &title);

    void xChanged(int arg);
    void yChanged(int arg);

    void widthChanged(int arg);
    void heightChanged(int arg);

    void minimumWidthChanged(int arg);
    void minimumHeightChanged(int arg);
    void maximumWidthChanged(int arg);
    void maximumHeightChanged(int arg);

    void visibleChanged(bool arg);
    Q_REVISION(1) void visibilityChanged(QWindow::Visibility visibility);
    Q_REVISION(1) void activeChanged();
    void contentOrientationChanged(Qt::ScreenOrientation orientation);

    void focusObjectChanged(QObject *object);

    Q_REVISION(1) void opacityChanged(qreal opacity);

protected:
    virtual void exposeEvent(QExposeEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void moveEvent(QMoveEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);

    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);
    // TODO Qt 6 - add closeEvent virtual handler

    virtual bool event(QEvent *) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent *);
#endif
    virtual void touchEvent(QTouchEvent *);
#ifndef QT_NO_TABLETEVENT
    virtual void tabletEvent(QTabletEvent *);
#endif
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    QWindow(QWindowPrivate &dd, QWindow *parent);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_clearAlert())
    QPlatformSurface *surfaceHandle() const Q_DECL_OVERRIDE;

    Q_DISABLE_COPY(QWindow)

    friend class QGuiApplication;
    friend class QGuiApplicationPrivate;
    friend class QWindowContainer;
    friend Q_GUI_EXPORT QWindowPrivate *qt_window_private(QWindow *window);
};

#ifndef Q_QDOC
template <> inline QWindow *qobject_cast<QWindow*>(QObject *o)
{
    if (!o || !o->isWindowType()) return Q_NULLPTR;
    return static_cast<QWindow*>(o);
}
template <> inline const QWindow *qobject_cast<const QWindow*>(const QObject *o)
{
    if (!o || !o->isWindowType()) return Q_NULLPTR;
    return static_cast<const QWindow*>(o);
}
#endif // !Q_QDOC

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QWindow *);
#endif

QT_END_NAMESPACE

#endif // QWINDOW_H
