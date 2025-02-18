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

#ifndef WL_REGION_H
#define WL_REGION_H

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

#include <QtWaylandCompositor/qtwaylandcompositorglobal.h>

#include <QRegion>

#include <wayland-util.h>
#include <QtWaylandCompositor/private/qwayland-server-wayland.h>

QT_BEGIN_NAMESPACE

namespace QtWayland {

class Q_WAYLAND_COMPOSITOR_EXPORT Region : public QtWaylandServer::wl_region
{
public:
    Region(struct wl_client *client, uint32_t id);
    ~Region();

    static Region *fromResource(struct ::wl_resource *resource);

    uint id() const { return resource()->handle->object.id; }

    QRegion region() const { return m_region; }

private:
    Q_DISABLE_COPY(Region)

    QRegion m_region;

    void region_destroy_resource(Resource *) Q_DECL_OVERRIDE;

    void region_destroy(Resource *resource) Q_DECL_OVERRIDE;
    void region_add(Resource *resource, int32_t x, int32_t y, int32_t w, int32_t h) Q_DECL_OVERRIDE;
    void region_subtract(Resource *resource, int32_t x, int32_t y, int32_t w, int32_t h) Q_DECL_OVERRIDE;
};

}

QT_END_NAMESPACE

#endif // WL_REGION_H

