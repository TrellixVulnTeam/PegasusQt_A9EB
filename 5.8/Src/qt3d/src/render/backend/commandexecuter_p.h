/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef QT3DRENDER_DEBUG_COMMANDEXECUTER_H
#define QT3DRENDER_DEBUG_COMMANDEXECUTER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifdef QT3D_JOBS_RUN_STATS

#include <QVector>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace Debug {
class AsynchronousCommandReply;
} // Debug

} // Qt3DCore

namespace Qt3DRender {

namespace Render {
class Renderer;
class RenderView;
} // Render

namespace Debug {

class CommandExecuter
{
public:
    explicit CommandExecuter(Render::Renderer *renderer);

    void performAsynchronousCommandExecution(const QVector<Render::RenderView *> &views);

    QVariant executeCommand(const QStringList &args);

private:
    Render::Renderer *m_renderer;
    QVector<Qt3DCore::Debug::AsynchronousCommandReply *> m_pendingCommands;
};

} // Debug

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3D_JOBS_RUN_STATS

#endif // QT3DRENDER_DEBUG_COMMANDEXECUTER_H
