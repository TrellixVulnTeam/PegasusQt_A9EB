/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
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

#ifndef RENDER_VIEW_OBSERVER_HOST_QT_H
#define RENDER_VIEW_OBSERVER_HOST_QT_H

#include "content/public/browser/web_contents_observer.h"

#include <QtGlobal>

namespace content {
    class WebContents;
}

namespace QtWebEngineCore {

class WebContentsAdapterClient;

class RenderViewObserverHostQt : public content::WebContentsObserver
{
public:
    RenderViewObserverHostQt(content::WebContents*, WebContentsAdapterClient *adapterClient);
    void fetchDocumentMarkup(quint64 requestId);
    void fetchDocumentInnerText(quint64 requestId);

private:
    bool OnMessageReceived(const IPC::Message& message) Q_DECL_OVERRIDE;
    void onDidFetchDocumentMarkup(quint64 requestId, const base::string16& markup);
    void onDidFetchDocumentInnerText(quint64 requestId, const base::string16& innerText);

    WebContentsAdapterClient *m_adapterClient;
};

} // namespace QtWebEngineCore

#endif // RENDER_VIEW_OBSERVER_HOST_QT_H
