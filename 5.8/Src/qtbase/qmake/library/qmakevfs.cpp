/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the qmake application of the Qt Toolkit.
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

#include "qmakevfs.h"

#include "ioutils.h"
using namespace QMakeInternal;

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>

#define fL1S(s) QString::fromLatin1(s)

QT_BEGIN_NAMESPACE

QMakeVfs::QMakeVfs()
#ifndef PROEVALUATOR_FULL
    : m_magicMissing(fL1S("missing"))
    , m_magicExisting(fL1S("existing"))
#endif
{
}

bool QMakeVfs::writeFile(const QString &fn, QIODevice::OpenMode mode, bool exe,
                         const QString &contents, QString *errStr)
{
#ifndef PROEVALUATOR_FULL
# ifdef PROEVALUATOR_THREAD_SAFE
    QMutexLocker locker(&m_mutex);
# endif
    QString *cont = &m_files[fn];
    if (mode & QIODevice::Append)
        *cont += contents;
    else
        *cont = contents;
    Q_UNUSED(errStr)
    Q_UNUSED(exe)
    return true;
#else
    QFileInfo qfi(fn);
    if (!QDir::current().mkpath(qfi.path())) {
        *errStr = fL1S("Cannot create parent directory");
        return false;
    }
    QByteArray bytes = contents.toLocal8Bit();
    QFile cfile(fn);
    if (!(mode & QIODevice::Append) && cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (cfile.readAll() == bytes) {
            if (exe) {
                cfile.setPermissions(cfile.permissions()
                                     | QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther);
            } else {
                cfile.setPermissions(cfile.permissions()
                                     & ~(QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther));
            }
            return true;
        }
        cfile.close();
    }
    if (!cfile.open(mode | QIODevice::WriteOnly | QIODevice::Text)) {
        *errStr = cfile.errorString();
        return false;
    }
    cfile.write(bytes);
    cfile.close();
    if (cfile.error() != QFile::NoError) {
        *errStr = cfile.errorString();
        return false;
    }
    if (exe)
        cfile.setPermissions(cfile.permissions()
                             | QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther);
    return true;
#endif
}

bool QMakeVfs::readFile(const QString &fn, QString *contents, QString *errStr)
{
#ifndef PROEVALUATOR_FULL
# ifdef PROEVALUATOR_THREAD_SAFE
    QMutexLocker locker(&m_mutex);
# endif
    QHash<QString, QString>::ConstIterator it = m_files.constFind(fn);
    if (it != m_files.constEnd()) {
        if (it->constData() == m_magicMissing.constData()) {
            *errStr = fL1S("No such file or directory");
            return false;
        }
        if (it->constData() != m_magicExisting.constData()) {
            *contents = *it;
            return true;
        }
    }
#endif

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly)) {
#ifndef PROEVALUATOR_FULL
        if (!IoUtils::exists(fn)) {
            m_files[fn] = m_magicMissing;
            *errStr = fL1S("No such file or directory");
        } else
#endif
            *errStr = file.errorString();
        return false;
    }
#ifndef PROEVALUATOR_FULL
    m_files[fn] = m_magicExisting;
#endif

    QByteArray bcont = file.readAll();
    if (bcont.startsWith("\xef\xbb\xbf")) {
        // UTF-8 BOM will cause subtle errors
        *errStr = fL1S("Unexpected UTF-8 BOM");
        return false;
    }
    *contents = QString::fromLocal8Bit(bcont);
    return true;
}

bool QMakeVfs::exists(const QString &fn)
{
#ifndef PROEVALUATOR_FULL
# ifdef PROEVALUATOR_THREAD_SAFE
    QMutexLocker locker(&m_mutex);
# endif
    QHash<QString, QString>::ConstIterator it = m_files.constFind(fn);
    if (it != m_files.constEnd())
        return it->constData() != m_magicMissing.constData();
#endif
    bool ex = IoUtils::exists(fn);
#ifndef PROEVALUATOR_FULL
    m_files[fn] = ex ? m_magicExisting : m_magicMissing;
#endif
    return ex;
}

#ifndef PROEVALUATOR_FULL
// This should be called when the sources may have changed (e.g., VCS update).
void QMakeVfs::invalidateCache()
{
# ifdef PROEVALUATOR_THREAD_SAFE
    QMutexLocker locker(&m_mutex);
# endif
    QHash<QString, QString>::Iterator it = m_files.begin(), eit = m_files.end();
    while (it != eit) {
        if (it->constData() == m_magicMissing.constData()
                ||it->constData() == m_magicExisting.constData())
            it = m_files.erase(it);
        else
            ++it;
    }
}

// This should be called when generated files may have changed (e.g., actual build).
void QMakeVfs::invalidateContents()
{
# ifdef PROEVALUATOR_THREAD_SAFE
    QMutexLocker locker(&m_mutex);
# endif
    m_files.clear();
}
#endif

QT_END_NAMESPACE
