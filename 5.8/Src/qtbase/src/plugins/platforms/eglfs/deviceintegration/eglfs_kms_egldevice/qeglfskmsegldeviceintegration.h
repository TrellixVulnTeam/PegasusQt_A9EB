/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Pelagicore AG
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

#ifndef QEGLFSKMSEGLDEVICEINTEGRATION_H
#define QEGLFSKMSEGLDEVICEINTEGRATION_H

#include <qeglfskmsintegration.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <QtEglSupport/private/qeglstreamconvenience_p.h>

QT_BEGIN_NAMESPACE

class QEglFSKmsEglDeviceIntegration : public QEglFSKmsIntegration
{
public:
    QEglFSKmsEglDeviceIntegration();

    EGLint surfaceType() const Q_DECL_OVERRIDE;
    EGLDisplay createDisplay(EGLNativeDisplayType nativeDisplay) Q_DECL_OVERRIDE;
    bool supportsSurfacelessContexts() const Q_DECL_OVERRIDE;
    bool supportsPBuffers() const Q_DECL_OVERRIDE;
    QEglFSWindow *createWindow(QWindow *window) const Q_DECL_OVERRIDE;

    EGLDeviceEXT eglDevice() const { return m_egl_device; }

protected:
    QEglFSKmsDevice *createDevice(const QString &devicePath) Q_DECL_OVERRIDE;
    QPlatformCursor *createCursor(QPlatformScreen *screen) const Q_DECL_OVERRIDE;

private:
    bool setup_kms();
    bool query_egl_device();

    EGLDeviceEXT m_egl_device;

    friend class QEglJetsonTK1Window;
    // EGLStream infrastructure
    QEGLStreamConvenience *m_funcs;
};

QT_END_NAMESPACE

#endif
