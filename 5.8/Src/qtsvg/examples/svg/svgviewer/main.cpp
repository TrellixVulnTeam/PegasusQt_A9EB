/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#ifndef QT_NO_OPENGL
#include <QGLFormat>
#endif

#include "mainwindow.h"

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(svgviewer);

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("SVG Viewer");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt SVG Viewer");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    MainWindow window;
    if (!window.loadFile(parser.positionalArguments().value(0, QLatin1String(":/files/bubbles.svg"))))
        return -1;
    window.show();
    return app.exec();
}
