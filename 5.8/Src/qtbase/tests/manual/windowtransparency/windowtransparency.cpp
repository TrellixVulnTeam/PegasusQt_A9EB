/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtGui>
#include <QtWidgets>

class GLWindow : public QWindow
{
public:
    GLWindow(Qt::WindowFlags flags)
        : gl(0)
    {
        setFlags(flags);
        setSurfaceType(OpenGLSurface);

        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        format.setSamples(16);
        setFormat(format);
    }

    void exposeEvent(QExposeEvent *)
    {
        if (!isExposed())
            return;

        if (!gl) {
            gl = new QOpenGLContext();
            gl->setFormat(requestedFormat());
            gl->create();
        }

        gl->makeCurrent(this);

        QOpenGLShaderProgram prog;
        prog.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                     "attribute highp vec4 a_Pos;"
                                     "attribute lowp vec4 a_Color;"
                                     "varying lowp vec4 v_Color;"
                                     "void main() {"
                                     "    gl_Position = a_Pos;"
                                     "    v_Color = a_Color;"
                                     "}");
        prog.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                     "varying lowp vec4 v_Color;"
                                     "void main() {"
                                     "    gl_FragColor = v_Color;"
                                     "}");
        prog.bind();

        QOpenGLFunctions *functions = gl->functions();
        functions->glClearColor(0, 0, 0, 0);
        functions->glClear(GL_COLOR_BUFFER_BIT);
        functions->glViewport(0, 0, width(), height());

        prog.enableAttributeArray("a_Pos");
        prog.enableAttributeArray("a_Color");

        float coords[] = { -0.7f,  0.7f,
                            0.8f,  0.8f,
                           -0.8f, -0.8f,
                            0.7f, -0.7f };
        float colors[] = { 1, 0, 0, 1,
                           0, 1, 0, 1,
                           0, 0, 1, 1,
                           0, 0, 0, 0 };

        prog.setAttributeArray("a_Pos", GL_FLOAT, coords, 2, 0);
        prog.setAttributeArray("a_Color", GL_FLOAT, colors, 4, 0);

        functions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        prog.disableAttributeArray("a_Pos");
        prog.disableAttributeArray("a_Color");

        gl->swapBuffers(this);
    }

    void mousePressEvent(QMouseEvent *)
    {
        QCoreApplication::quit();
    }

private:
    QOpenGLContext *gl;
};

class Widget : public QWidget
{
public:
    Widget(Qt::WindowFlags flags)
        : QWidget(0, flags)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);

        p.setRenderHint(QPainter::Antialiasing);

        p.fillRect(rect(), QColor("steelblue"));

        int w = width();
        int h = height();
        int w2 = width() / 2;
        int h2 = height() / 2;

        QPainterPath path;
        path.moveTo(0, 0);
        path.cubicTo(w2, 0,  w2, h2, w, h);
        path.cubicTo(w2, h, w2, h2, 0, 0);

        QLinearGradient lg(0, 0, w, h);
        lg.setColorAt(0.0, Qt::transparent);
        lg.setColorAt(0.5, QColor("palegreen"));
        lg.setColorAt(1.0, Qt::transparent);

        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillPath(path, lg);
    }

    void mousePressEvent(QMouseEvent *)
    {
        QCoreApplication::quit();
    }

};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // mask: on/off
    // opacity: on/off

    for (int i=0; i<4; ++i) {
        bool mask = i & 0x1;
        bool opacity = i & 0x2;

        Qt::WindowFlags flags = Qt::FramelessWindowHint;

        Widget *widget = new Widget(flags);
        GLWindow *window = new GLWindow(flags);

        widget->setGeometry(100 + 100 * i, 100, 80, 80);
        window->setGeometry(100 + 100 * i, 200, 80, 80);
        if (mask) {
            QRegion region(0, 0, 80, 80, QRegion::Ellipse);
            widget->setMask(region);
            window->setMask(region);
        }
        if (opacity) {
            widget->setWindowOpacity(0.5);
            window->setOpacity(0.5);
        }

        widget->show();
        window->show();
    }

    return app.exec();
}
