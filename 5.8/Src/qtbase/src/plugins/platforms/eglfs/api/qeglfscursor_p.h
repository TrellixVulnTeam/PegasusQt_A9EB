/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QEGLFSCURSOR_H
#define QEGLFSCURSOR_H

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

#include "qeglfsglobal_p.h"
#include <qpa/qplatformcursor.h>
#include <qpa/qplatformscreen.h>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/private/qinputdevicemanager_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;
class QEglFSCursor;
class QEglFSScreen;

class QEglFSCursorDeviceListener : public QObject
{
    Q_OBJECT

public:
    QEglFSCursorDeviceListener(QEglFSCursor *cursor) : m_cursor(cursor) { }
    bool hasMouse() const;

public slots:
    void onDeviceListChanged(QInputDeviceManager::DeviceType type);

private:
    QEglFSCursor *m_cursor;
};

class Q_EGLFS_EXPORT QEglFSCursor : public QPlatformCursor, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    QEglFSCursor(QPlatformScreen *screen);
    ~QEglFSCursor();

#ifndef QT_NO_CURSOR
    void changeCursor(QCursor *cursor, QWindow *widget) Q_DECL_OVERRIDE;
#endif
    void pointerEvent(const QMouseEvent &event) Q_DECL_OVERRIDE;
    QPoint pos() const Q_DECL_OVERRIDE;
    void setPos(const QPoint &pos) Q_DECL_OVERRIDE;

    QRect cursorRect() const;
    void paintOnScreen();
    void resetResources();

    void updateMouseStatus();

private:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
#ifndef QT_NO_CURSOR
    bool setCurrentCursor(QCursor *cursor);
#endif
    void draw(const QRectF &rect);
    void update(const QRect &rect, bool allScreens);
    void createShaderPrograms();
    void createCursorTexture(uint *texture, const QImage &image);
    void initCursorAtlas();

    // current cursor information
    struct Cursor {
        Cursor() : shape(Qt::BlankCursor), customCursorPending(false), customCursorKey(0), useCustomCursor(false) { }
        Qt::CursorShape shape;
        QRectF textureRect; // normalized rect inside texture
        QSize size; // size of the cursor
        QPoint hotSpot;
        QImage customCursorImage;
        QPoint pos; // current cursor position
        bool customCursorPending;
        qint64 customCursorKey;
        bool useCustomCursor;
    } m_cursor;

    // cursor atlas information
    struct CursorAtlas {
        CursorAtlas() : cursorsPerRow(0), cursorWidth(0), cursorHeight(0) { }
        int cursorsPerRow;
        int width, height; // width and height of the atlas
        int cursorWidth, cursorHeight; // width and height of cursors inside the atlas
        QList<QPoint> hotSpots;
        QImage image; // valid until it's uploaded
    } m_cursorAtlas;

    bool m_visible;
    QEglFSScreen *m_screen;
    QPlatformScreen *m_activeScreen;
    QEglFSCursorDeviceListener *m_deviceListener;
    bool m_updateRequested;
    QMatrix4x4 m_rotationMatrix;

    struct GraphicsContextData {
        GraphicsContextData() : program(nullptr), textureEntry(0), matEntry(0),
            customCursorTexture(0), atlasTexture(0), customCursorKey(0) { }
        QOpenGLShaderProgram *program;
        int textureEntry;
        int matEntry;
        uint customCursorTexture;
        uint atlasTexture;
        qint64 customCursorKey;
    };
    QHash<QOpenGLContext *, GraphicsContextData> m_gfx;
};

QT_END_NAMESPACE

#endif // QEGLFSCURSOR_H
