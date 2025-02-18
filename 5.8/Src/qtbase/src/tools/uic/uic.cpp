/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "uic.h"
#include "ui4.h"
#include "driver.h"
#include "option.h"
#include "treewalker.h"
#include "validator.h"

#ifdef QT_UIC_CPP_GENERATOR
#include "cppwriteincludes.h"
#include "cppwritedeclaration.h"
#endif

#ifdef QT_UIC_JAVA_GENERATOR
#include "javawriteincludes.h"
#include "javawritedeclaration.h"
#endif

#include <qxmlstream.h>
#include <qfileinfo.h>
#include <qtextstream.h>

QT_BEGIN_NAMESPACE

Uic::Uic(Driver *d)
     : drv(d),
       out(d->output()),
       opt(d->option()),
       externalPix(true)
{
}

Uic::~Uic()
{
}

bool Uic::printDependencies()
{
    QString fileName = opt.inputFile;

    QFile f;
    if (fileName.isEmpty())
        f.open(stdin, QIODevice::ReadOnly);
    else {
        f.setFileName(fileName);
        if (!f.open(QIODevice::ReadOnly))
            return false;
    }

    DomUI *ui = 0;
    {
        QXmlStreamReader reader;
        reader.setDevice(&f);
        ui = parseUiFile(reader);
        if (!ui)
            return false;
    }

    if (DomIncludes *includes = ui->elementIncludes()) {
        const auto incls = includes->elementInclude();
        for (DomInclude *incl : incls) {
            QString file = incl->text();
            if (file.isEmpty())
                continue;

            fprintf(stdout, "%s\n", file.toLocal8Bit().constData());
        }
    }

    if (DomCustomWidgets *customWidgets = ui->elementCustomWidgets()) {
        const auto elementCustomWidget = customWidgets->elementCustomWidget();
        for (DomCustomWidget *customWidget : elementCustomWidget) {
            if (DomHeader *header = customWidget->elementHeader()) {
                QString file = header->text();
                if (file.isEmpty())
                    continue;

                fprintf(stdout, "%s\n", file.toLocal8Bit().constData());
            }
        }
    }

    delete ui;

    return true;
}

void Uic::writeCopyrightHeader(DomUI *ui)
{
    QString comment = ui->elementComment();
    if (comment.size())
        out << "/*\n" << comment << "\n*/\n\n";

    out << "/********************************************************************************\n";
    out << "** Form generated from reading UI file '" << QFileInfo(opt.inputFile).fileName() << "'\n";
    out << "**\n";
    out << "** Created by: Qt User Interface Compiler version " << QLatin1String(QT_VERSION_STR) << "\n";
    out << "**\n";
    out << "** WARNING! All changes made in this file will be lost when recompiling UI file!\n";
    out << "********************************************************************************/\n\n";
}

// Check the version with a stream reader at the <ui> element.

static double versionFromUiAttribute(QXmlStreamReader &reader)
{
    const QXmlStreamAttributes attributes = reader.attributes();
    const QString versionAttribute = QLatin1String("version");
    if (!attributes.hasAttribute(versionAttribute))
        return 4.0;
    const QStringRef version = attributes.value(versionAttribute);
    return version.toDouble();
}

DomUI *Uic::parseUiFile(QXmlStreamReader &reader)
{
    DomUI *ui = 0;

    const QString uiElement = QLatin1String("ui");
    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
            if (reader.name().compare(uiElement, Qt::CaseInsensitive) == 0
                && !ui) {
                const double version = versionFromUiAttribute(reader);
                if (version < 4.0) {
                    const QString msg = QString::fromLatin1("uic: File generated with too old version of Qt Designer (%1)").arg(version);
                    fprintf(stderr, "%s\n", qPrintable(msg));
                    return 0;
                }

                ui = new DomUI();
                ui->read(reader);
            } else {
                reader.raiseError(QLatin1String("Unexpected element ") + reader.name().toString());
            }
        }
    }
    if (reader.hasError()) {
        delete ui;
        ui = 0;
        fprintf(stderr, "%s\n", qPrintable(QString::fromLatin1("uic: Error in line %1, column %2 : %3")
                                    .arg(reader.lineNumber()).arg(reader.columnNumber())
                                    .arg(reader.errorString())));
    }

    return ui;
}

bool Uic::write(QIODevice *in)
{
    if (option().generator == Option::JavaGenerator) {
         // the Java generator ignores header protection
        opt.headerProtection = false;
    }

    DomUI *ui = 0;
    {
        QXmlStreamReader reader;
        reader.setDevice(in);
        ui = parseUiFile(reader);

        if (!ui)
            return false;
    }

    double version = ui->attributeVersion().toDouble();
    if (version < 4.0) {
        delete ui;

        fprintf(stderr, "uic: File generated with too old version of Qt Designer\n");
        return false;
    }

    QString language = ui->attributeLanguage();


    bool rtn = false;

    if (option().generator == Option::JavaGenerator) {
#ifdef QT_UIC_JAVA_GENERATOR
        if (language.toLower() != QLatin1String("jambi")) {
            fprintf(stderr, "uic: File is not a 'jambi' form\n");
            delete ui;
            return false;
        }
        rtn = jwrite (ui);
#else
        fprintf(stderr, "uic: option to generate java code not compiled in\n");
#endif
    } else {
#ifdef QT_UIC_CPP_GENERATOR
        if (!language.isEmpty() && language.toLower() != QLatin1String("c++")) {
            fprintf(stderr, "uic: File is not a 'c++' ui file, language=%s\n", qPrintable(language));
            delete ui;
            return false;
        }

        rtn = write (ui);
#else
        fprintf(stderr, "uic: option to generate cpp code not compiled in\n");
#endif
    }

    delete ui;

    return rtn;
}

#ifdef QT_UIC_CPP_GENERATOR
bool Uic::write(DomUI *ui)
{
    using namespace CPP;

    if (!ui || !ui->elementWidget())
        return false;

    if (opt.copyrightHeader)
        writeCopyrightHeader(ui);

    if (opt.headerProtection) {
        writeHeaderProtectionStart();
        out << "\n";
    }

    pixFunction = ui->elementPixmapFunction();
    if (pixFunction == QLatin1String("QPixmap::fromMimeSource"))
        pixFunction = QLatin1String("qPixmapFromMimeSource");

    externalPix = ui->elementImages() == 0;

    info.acceptUI(ui);
    cWidgetsInfo.acceptUI(ui);
    WriteIncludes writeIncludes(this);
    writeIncludes.acceptUI(ui);

    Validator(this).acceptUI(ui);
    WriteDeclaration(this, writeIncludes.scriptsActivated()).acceptUI(ui);

    if (opt.headerProtection)
        writeHeaderProtectionEnd();

    return true;
}
#endif

#ifdef QT_UIC_JAVA_GENERATOR
bool Uic::jwrite(DomUI *ui)
{
    using namespace Java;

    if (!ui || !ui->elementWidget())
        return false;

    if (opt.copyrightHeader)
        writeCopyrightHeader(ui);

    pixFunction = ui->elementPixmapFunction();
    if (pixFunction == QLatin1String("QPixmap::fromMimeSource"))
        pixFunction = QLatin1String("qPixmapFromMimeSource");

    externalPix = ui->elementImages() == 0;

    info.acceptUI(ui);
    cWidgetsInfo.acceptUI(ui);
    WriteIncludes(this).acceptUI(ui);

    Validator(this).acceptUI(ui);
    WriteDeclaration(this).acceptUI(ui);

    return true;
}
#endif

#ifdef QT_UIC_CPP_GENERATOR

void Uic::writeHeaderProtectionStart()
{
    QString h = drv->headerFileName();
    out << "#ifndef " << h << "\n"
        << "#define " << h << "\n";
}

void Uic::writeHeaderProtectionEnd()
{
    QString h = drv->headerFileName();
    out << "#endif // " << h << "\n";
}
#endif

bool Uic::isMainWindow(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QMainWindow"));
}

bool Uic::isToolBar(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QToolBar"));
}

bool Uic::isButton(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QRadioButton"))
        || customWidgetsInfo()->extends(className, QLatin1String("QToolButton"))
        || customWidgetsInfo()->extends(className, QLatin1String("QCheckBox"))
        || customWidgetsInfo()->extends(className, QLatin1String("QPushButton"))
        || customWidgetsInfo()->extends(className, QLatin1String("QCommandLinkButton"));
}

bool Uic::isContainer(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QStackedWidget"))
        || customWidgetsInfo()->extends(className, QLatin1String("QToolBox"))
        || customWidgetsInfo()->extends(className, QLatin1String("QTabWidget"))
        || customWidgetsInfo()->extends(className, QLatin1String("QScrollArea"))
        || customWidgetsInfo()->extends(className, QLatin1String("QMdiArea"))
        || customWidgetsInfo()->extends(className, QLatin1String("QWizard"))
        || customWidgetsInfo()->extends(className, QLatin1String("QDockWidget"));
}

bool Uic::isCustomWidgetContainer(const QString &className) const
{
    return customWidgetsInfo()->isCustomWidgetContainer(className);
}

bool Uic::isStatusBar(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QStatusBar"));
}

bool Uic::isMenuBar(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QMenuBar"));
}

bool Uic::isMenu(const QString &className) const
{
    return customWidgetsInfo()->extends(className, QLatin1String("QMenu"))
        || customWidgetsInfo()->extends(className, QLatin1String("QPopupMenu"));
}

QT_END_NAMESPACE
