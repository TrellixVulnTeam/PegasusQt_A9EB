/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QMENU_H
#define QMENU_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qwidget.h>
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qaction.h>

#ifdef Q_OS_OSX
Q_FORWARD_DECLARE_OBJC_CLASS(NSMenu);
#endif

QT_BEGIN_NAMESPACE


#ifndef QT_NO_MENU

class QMenuPrivate;
class QStyleOptionMenuItem;
class QPlatformMenu;

class Q_WIDGETS_EXPORT QMenu : public QWidget
{
private:
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMenu)

    Q_PROPERTY(bool tearOffEnabled READ isTearOffEnabled WRITE setTearOffEnabled)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool separatorsCollapsible READ separatorsCollapsible WRITE setSeparatorsCollapsible)
    Q_PROPERTY(bool toolTipsVisible READ toolTipsVisible WRITE setToolTipsVisible)

public:
    explicit QMenu(QWidget *parent = Q_NULLPTR);
    explicit QMenu(const QString &title, QWidget *parent = Q_NULLPTR);
    ~QMenu();

    using QWidget::addAction;
    QAction *addAction(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addAction(const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut = 0);
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut = 0);

#ifdef Q_QDOC
    template<typename PointerToMemberFunction>
    QAction *addAction(const QString &text, const QObject *receiver, PointerToMemberFunction method, const QKeySequence &shortcut = 0);
    template<typename Functor>
    QAction *addAction(const QString &text, Functor functor, const QKeySequence &shortcut = 0);
    template<typename Functor>
    QAction *addAction(const QString &text, const QObject *context, Functor functor, const QKeySequence &shortcut = 0);
    template<typename PointerToMemberFunction>
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *receiver, PointerToMemberFunction method, const QKeySequence &shortcut = 0);
    template<typename Functor>
    QAction *addAction(const QIcon &icon, const QString &text, Functor functor, const QKeySequence &shortcut = 0);
    template<typename Functor>
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *context, Functor functor, const QKeySequence &shortcut = 0);
#else
    // addAction(QString): Connect to a QObject slot / functor or function pointer (with context)
    template<class Obj, typename Func1>
    inline typename QtPrivate::QEnableIf<!std::is_same<const char*, Func1>::value
        && QtPrivate::IsPointerToTypeDerivedFromQObject<Obj*>::Value, QAction *>::Type
        addAction(const QString &text, const Obj *object, Func1 slot, const QKeySequence &shortcut = 0)
    {
        QAction *result = addAction(text);
#ifdef QT_NO_SHORTCUT
        Q_UNUSED(shortcut)
#else
        result->setShortcut(shortcut);
#endif
        connect(result, &QAction::triggered, object, slot);
        return result;
    }
    // addAction(QString): Connect to a functor or function pointer (without context)
    template <typename Func1>
    inline QAction *addAction(const QString &text, Func1 slot, const QKeySequence &shortcut = 0)
    {
        QAction *result = addAction(text);
#ifdef QT_NO_SHORTCUT
        Q_UNUSED(shortcut)
#else
        result->setShortcut(shortcut);
#endif
        connect(result, &QAction::triggered, slot);
        return result;
    }
    // addAction(QIcon, QString): Connect to a QObject slot / functor or function pointer (with context)
    template<class Obj, typename Func1>
    inline typename QtPrivate::QEnableIf<!std::is_same<const char*, Func1>::value
        && QtPrivate::IsPointerToTypeDerivedFromQObject<Obj*>::Value, QAction *>::Type
        addAction(const QIcon &actionIcon, const QString &text, const Obj *object, Func1 slot, const QKeySequence &shortcut = 0)
    {
        QAction *result = addAction(actionIcon, text);
#ifdef QT_NO_SHORTCUT
        Q_UNUSED(shortcut)
#else
        result->setShortcut(shortcut);
#endif
        connect(result, &QAction::triggered, object, slot);
        return result;
    }
    // addAction(QIcon, QString): Connect to a functor or function pointer (without context)
    template <typename Func1>
    inline QAction *addAction(const QIcon &actionIcon, const QString &text, Func1 slot, const QKeySequence &shortcut = 0)
    {
        QAction *result = addAction(actionIcon, text);
#ifdef QT_NO_SHORTCUT
        Q_UNUSED(shortcut)
#else
        result->setShortcut(shortcut);
#endif
        connect(result, &QAction::triggered, slot);
        return result;
    }
#endif // !Q_QDOC

    QAction *addMenu(QMenu *menu);
    QMenu *addMenu(const QString &title);
    QMenu *addMenu(const QIcon &icon, const QString &title);

    QAction *addSeparator();

    QAction *addSection(const QString &text);
    QAction *addSection(const QIcon &icon, const QString &text);

    QAction *insertMenu(QAction *before, QMenu *menu);
    QAction *insertSeparator(QAction *before);
    QAction *insertSection(QAction *before, const QString &text);
    QAction *insertSection(QAction *before, const QIcon &icon, const QString &text);

    bool isEmpty() const;
    void clear();

    void setTearOffEnabled(bool);
    bool isTearOffEnabled() const;

    bool isTearOffMenuVisible() const;
    void showTearOffMenu();
    void showTearOffMenu(const QPoint &pos);
    void hideTearOffMenu();

    void setDefaultAction(QAction *);
    QAction *defaultAction() const;

    void setActiveAction(QAction *act);
    QAction *activeAction() const;

    void popup(const QPoint &pos, QAction *at = Q_NULLPTR);
    QAction *exec();
    QAction *exec(const QPoint &pos, QAction *at = Q_NULLPTR);

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    static QAction *exec(const QList<QAction *> &actions, const QPoint &pos, QAction *at = Q_NULLPTR, QWidget *parent = Q_NULLPTR);
#else
    static QAction *exec(QList<QAction*> actions, const QPoint &pos, QAction *at = Q_NULLPTR, QWidget *parent = Q_NULLPTR);
#endif

    QSize sizeHint() const Q_DECL_OVERRIDE;

    QRect actionGeometry(QAction *) const;
    QAction *actionAt(const QPoint &) const;

    QAction *menuAction() const;

    QString title() const;
    void setTitle(const QString &title);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    void setNoReplayFor(QWidget *widget);
    QPlatformMenu *platformMenu();
    void setPlatformMenu(QPlatformMenu *platformMenu);

#ifdef Q_OS_OSX
    NSMenu* toNSMenu();
    void setAsDockMenu();
#endif

    bool separatorsCollapsible() const;
    void setSeparatorsCollapsible(bool collapse);

    bool toolTipsVisible() const;
    void setToolTipsVisible(bool visible);

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();
    void triggered(QAction *action);
    void hovered(QAction *action);

protected:
    int columnCount() const;

    void changeEvent(QEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
#endif
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void actionEvent(QActionEvent *) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE;
    bool event(QEvent *) Q_DECL_OVERRIDE;
    bool focusNextPrevChild(bool next) Q_DECL_OVERRIDE;
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;

private Q_SLOTS:
    void internalDelayedPopup();

private:
    Q_PRIVATE_SLOT(d_func(), void _q_actionTriggered())
    Q_PRIVATE_SLOT(d_func(), void _q_actionHovered())
    Q_PRIVATE_SLOT(d_func(), void _q_overrideMenuActionDestroyed())
    Q_PRIVATE_SLOT(d_func(), void _q_platformMenuAboutToShow())

protected:
    QMenu(QMenuPrivate &dd, QWidget* parent = Q_NULLPTR);

private:
    Q_DISABLE_COPY(QMenu)

    friend class QMenuBar;
    friend class QMenuBarPrivate;
    friend class QTornOffMenu;
    friend class QComboBox;
    friend class QAction;
    friend class QToolButtonPrivate;
    friend void qt_mac_emit_menuSignals(QMenu *menu, bool show);
    friend void qt_mac_menu_emit_hovered(QMenu *menu, QAction *action);
};

#ifdef Q_OS_OSX
// ### Qt 4 compatibility; remove in Qt 6
inline QT_DEPRECATED void qt_mac_set_dock_menu(QMenu *menu) { menu->setAsDockMenu(); }
#endif

#endif // QT_NO_MENU

QT_END_NAMESPACE

#endif // QMENU_H
