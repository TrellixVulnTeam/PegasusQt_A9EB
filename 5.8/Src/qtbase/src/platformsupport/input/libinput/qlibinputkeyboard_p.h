/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins module of the Qt Toolkit.
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

#ifndef QLIBINPUTKEYBOARD_P_H
#define QLIBINPUTKEYBOARD_P_H

#include <QtCore/QPoint>
#include <QtCore/QTimer>

#ifndef QT_NO_XKBCOMMON_EVDEV
#include <xkbcommon/xkbcommon.h>
#endif

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

struct libinput_event_keyboard;

QT_BEGIN_NAMESPACE

class QLibInputKeyboard : public QObject
{
    Q_OBJECT

public:
    QLibInputKeyboard();
    ~QLibInputKeyboard();

    void processKey(libinput_event_keyboard *e);

#ifndef QT_NO_XKBCOMMON_EVDEV
private slots:
    void handleRepeat();

private:
    int keysymToQtKey(xkb_keysym_t key) const;
    int keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers *modifiers, const QString &text) const;

    xkb_context *m_ctx;
    xkb_keymap *m_keymap;
    xkb_state *m_state;
    xkb_mod_index_t m_modindex[4];

    QTimer m_repeatTimer;

    struct {
        int qtkey;
        Qt::KeyboardModifiers mods;
        int nativeScanCode;
        int virtualKey;
        int nativeMods;
        QString unicodeText;
        int repeatCount;
    } m_repeatData;
#endif
};

QT_END_NAMESPACE

#endif
