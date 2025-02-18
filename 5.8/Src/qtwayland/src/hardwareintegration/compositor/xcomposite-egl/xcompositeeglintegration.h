/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
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

#ifndef XCOMPOSITEEGLINTEGRATION_H
#define XCOMPOSITEEGLINTEGRATION_H

#include <QtWaylandCompositor/private/qwlclientbufferintegration_p.h>

#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/private/qwlclientbuffer_p.h>
#include "xlibinclude.h"

#include <EGL/egl.h>

QT_BEGIN_NAMESPACE

class XCompositeEglClientBufferIntegration : public QtWayland::ClientBufferIntegration
{
public:
    XCompositeEglClientBufferIntegration();

    void initializeHardware(struct ::wl_display *display) Q_DECL_OVERRIDE;
    QtWayland::ClientBuffer *createBufferFor(wl_resource *buffer) Q_DECL_OVERRIDE;
    inline Display *xDisplay() const { return mDisplay; }
    inline EGLDisplay eglDisplay() const { return mEglDisplay; }

private:
    Display *mDisplay;
    EGLDisplay mEglDisplay;
};

class XCompositeEglClientBuffer : public QtWayland::ClientBuffer
{
public:
    XCompositeEglClientBuffer(XCompositeEglClientBufferIntegration *integration, wl_resource *bufferResource);

    QSize size() const;
    QWaylandSurface::Origin origin() const;
    QOpenGLTexture *toOpenGlTexture(int plane) Q_DECL_OVERRIDE;
    QWaylandBufferRef::BufferFormatEgl bufferFormatEgl() const Q_DECL_OVERRIDE {
        return QWaylandBufferRef::BufferFormatEgl_RGBA;
    }

private:
    QOpenGLTexture *m_texture;
    XCompositeEglClientBufferIntegration *m_integration;
};

QT_END_NAMESPACE

#endif // XCOMPOSITEEGLINTEGRATION_H
