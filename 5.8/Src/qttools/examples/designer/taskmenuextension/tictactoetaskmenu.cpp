/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tictactoe.h"
#include "tictactoedialog.h"
#include "tictactoetaskmenu.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QAction>

//! [0]
TicTacToeTaskMenu::TicTacToeTaskMenu(TicTacToe *tic, QObject *parent)
    : QObject(parent)
    , editStateAction(new QAction(tr("Edit State..."), this))
    , ticTacToe(tic)
{
    connect(editStateAction, &QAction::triggered, this, &TicTacToeTaskMenu::editState);
}
//! [0]

//! [1]
void TicTacToeTaskMenu::editState()
{
    TicTacToeDialog dialog(ticTacToe);
    dialog.exec();
}
//! [1]

//! [2]
QAction *TicTacToeTaskMenu::preferredEditAction() const
{
    return editStateAction;
}
//! [2]

//! [3]
QList<QAction *> TicTacToeTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(editStateAction);
    return list;
}
//! [3]

//! [4]
TicTacToeTaskMenuFactory::TicTacToeTaskMenuFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}
//! [4]

//! [5]
QObject *TicTacToeTaskMenuFactory::createExtension(QObject *object,
                                                   const QString &iid,
                                                   QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return 0;

    if (TicTacToe *tic = qobject_cast<TicTacToe*>(object))
        return new TicTacToeTaskMenu(tic, parent);

    return 0;
}
//! [5]
