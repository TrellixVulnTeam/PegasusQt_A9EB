/****************************************************************************
**
** Copyright (C) 2016 Research In Motion
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
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
#ifndef PPSMEDIAPLAYERCONTROL_H
#define PPSMEDIAPLAYERCONTROL_H

#include "mmrenderermediaplayercontrol.h"

QT_BEGIN_NAMESPACE

class QSocketNotifier;

class PpsMediaPlayerControl Q_DECL_FINAL : public MmRendererMediaPlayerControl
{
    Q_OBJECT
public:
    explicit PpsMediaPlayerControl(QObject *parent = 0);
    ~PpsMediaPlayerControl();

    void startMonitoring(int contextId, const QString &contextName) Q_DECL_OVERRIDE;
    void stopMonitoring() Q_DECL_OVERRIDE;

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void ppsReadyRead(int fd);

private:
    QSocketNotifier *m_ppsStatusNotifier;
    int m_ppsStatusFd;
    QSocketNotifier *m_ppsStateNotifier;
    int m_ppsStateFd;
    QByteArray m_previouslySeenState;
};

QT_END_NAMESPACE

#endif
