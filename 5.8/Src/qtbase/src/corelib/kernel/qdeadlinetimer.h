/****************************************************************************
**
** Copyright (C) 2016 Intel Corporation.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
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
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDEADLINETIMER_H
#define QDEADLINETIMER_H

#include <QtCore/qelapsedtimer.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qnamespace.h>

#ifdef max
// un-pollute the namespace. We need std::numeric_limits::max() and std::chrono::duration::max()
#  undef max
#endif

#include <limits>

#if QT_HAS_INCLUDE(<chrono>)
#  include <chrono>
#endif

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QDeadlineTimer
{
public:
    enum ForeverConstant { Forever };

    Q_DECL_CONSTEXPR QDeadlineTimer(Qt::TimerType type_ = Qt::CoarseTimer) Q_DECL_NOTHROW
        : t1(0), t2(0), type(type_) {}
    Q_DECL_CONSTEXPR QDeadlineTimer(ForeverConstant, Qt::TimerType type_ = Qt::CoarseTimer) Q_DECL_NOTHROW
        : t1(std::numeric_limits<qint64>::max()), t2(0), type(type_) {}
    explicit QDeadlineTimer(qint64 msecs, Qt::TimerType type = Qt::CoarseTimer) Q_DECL_NOTHROW;

    void swap(QDeadlineTimer &other) Q_DECL_NOTHROW
    { qSwap(t1, other.t1); qSwap(t2, other.t2); qSwap(type, other.type); }

    Q_DECL_CONSTEXPR bool isForever() const Q_DECL_NOTHROW
    { return t1 == (std::numeric_limits<qint64>::max)(); }
    bool hasExpired() const Q_DECL_NOTHROW;

    Qt::TimerType timerType() const Q_DECL_NOTHROW
    { return Qt::TimerType(type & 0xff); }
    void setTimerType(Qt::TimerType type);

    qint64 remainingTime() const Q_DECL_NOTHROW;
    qint64 remainingTimeNSecs() const Q_DECL_NOTHROW;
    void setRemainingTime(qint64 msecs, Qt::TimerType type = Qt::CoarseTimer) Q_DECL_NOTHROW;
    void setPreciseRemainingTime(qint64 secs, qint64 nsecs = 0,
                                 Qt::TimerType type = Qt::CoarseTimer) Q_DECL_NOTHROW;

    qint64 deadline() const Q_DECL_NOTHROW Q_DECL_PURE_FUNCTION;
    qint64 deadlineNSecs() const Q_DECL_NOTHROW Q_DECL_PURE_FUNCTION;
    void setDeadline(qint64 msecs, Qt::TimerType timerType = Qt::CoarseTimer) Q_DECL_NOTHROW;
    void setPreciseDeadline(qint64 secs, qint64 nsecs = 0,
                            Qt::TimerType type = Qt::CoarseTimer) Q_DECL_NOTHROW;

    static QDeadlineTimer addNSecs(QDeadlineTimer dt, qint64 nsecs) Q_DECL_NOTHROW Q_DECL_PURE_FUNCTION;
    static QDeadlineTimer current(Qt::TimerType timerType = Qt::CoarseTimer) Q_DECL_NOTHROW;

    friend bool operator==(QDeadlineTimer d1, QDeadlineTimer d2) Q_DECL_NOTHROW
    { return d1.t1 == d2.t1 && d1.t2 == d2.t2; }
    friend bool operator!=(QDeadlineTimer d1, QDeadlineTimer d2) Q_DECL_NOTHROW
    { return !(d1 == d2); }
    friend bool operator<(QDeadlineTimer d1, QDeadlineTimer d2) Q_DECL_NOTHROW
    { return d1.t1 < d2.t1 || (d1.t1 == d2.t1 && d1.t2 < d2.t2); }
    friend bool operator<=(QDeadlineTimer d1, QDeadlineTimer d2) Q_DECL_NOTHROW
    { return d1 == d2 || d1 < d2; }
    friend bool operator>(QDeadlineTimer d1, QDeadlineTimer d2) Q_DECL_NOTHROW
    { return d2 < d1; }
    friend bool operator>=(QDeadlineTimer d1, QDeadlineTimer d2) Q_DECL_NOTHROW
    { return !(d1 < d2); }

    friend QDeadlineTimer operator+(QDeadlineTimer dt, qint64 msecs)
    { return QDeadlineTimer::addNSecs(dt, msecs * 1000 * 1000); }
    friend QDeadlineTimer operator+(qint64 msecs, QDeadlineTimer dt)
    { return dt + msecs; }
    friend QDeadlineTimer operator-(QDeadlineTimer dt, qint64 msecs)
    { return dt + (-msecs); }
    friend qint64 operator-(QDeadlineTimer dt1, QDeadlineTimer dt2)
    { return (dt1.deadlineNSecs() - dt2.deadlineNSecs()) / (1000 * 1000); }
    QDeadlineTimer &operator+=(qint64 msecs)
    { *this = *this + msecs; return *this; }
    QDeadlineTimer &operator-=(qint64 msecs)
    { *this = *this + (-msecs); return *this; }

#if QT_HAS_INCLUDE(<chrono>) || defined(Q_QDOC)
    template <class Clock, class Duration>
    QDeadlineTimer(std::chrono::time_point<Clock, Duration> deadline_,
                   Qt::TimerType type_ = Qt::CoarseTimer) : t2(0)
    { setDeadline(deadline_, type_); }
    template <class Clock, class Duration>
    QDeadlineTimer &operator=(std::chrono::time_point<Clock, Duration> deadline_)
    { setDeadline(deadline_); return *this; }

    template <class Clock, class Duration>
    void setDeadline(std::chrono::time_point<Clock, Duration> deadline_,
                     Qt::TimerType type_ = Qt::CoarseTimer)
    { setRemainingTime(deadline_ == deadline_.max() ? Duration::max() : deadline_ - Clock::now(), type_); }

    template <class Clock, class Duration = typename Clock::duration>
    std::chrono::time_point<Clock, Duration> deadline() const
    {
        auto val = std::chrono::nanoseconds(rawRemainingTimeNSecs()) + Clock::now();
        return std::chrono::time_point_cast<Duration>(val);
    }

    template <class Rep, class Period>
    QDeadlineTimer(std::chrono::duration<Rep, Period> remaining, Qt::TimerType type_ = Qt::CoarseTimer)
        : t2(0)
    { setRemainingTime(remaining, type_); }

    template <class Rep, class Period>
    QDeadlineTimer &operator=(std::chrono::duration<Rep, Period> remaining)
    { setRemainingTime(remaining); return *this; }

    template <class Rep, class Period>
    void setRemainingTime(std::chrono::duration<Rep, Period> remaining, Qt::TimerType type_ = Qt::CoarseTimer)
    {
        if (remaining == remaining.max())
            *this = QDeadlineTimer(Forever, type_);
        else
            setPreciseRemainingTime(0, std::chrono::nanoseconds(remaining).count(), type_);
    }

    std::chrono::nanoseconds remainingTimeAsDuration() const Q_DECL_NOTHROW
    {
        if (isForever())
            return std::chrono::nanoseconds::max();
        qint64 nsecs = rawRemainingTimeNSecs();
        if (nsecs <= 0)
            return std::chrono::nanoseconds::zero();
        return std::chrono::nanoseconds(nsecs);
    }

    template <class Rep, class Period>
    friend QDeadlineTimer operator+(QDeadlineTimer dt, std::chrono::duration<Rep, Period> value)
    { return QDeadlineTimer::addNSecs(dt, std::chrono::duration_cast<std::chrono::nanoseconds>(value).count()); }
    template <class Rep, class Period>
    friend QDeadlineTimer operator+(std::chrono::duration<Rep, Period> value, QDeadlineTimer dt)
    { return dt + value; }
    template <class Rep, class Period>
    friend QDeadlineTimer operator+=(QDeadlineTimer &dt, std::chrono::duration<Rep, Period> value)
    { return dt = dt + value; }
#endif

private:
    qint64 t1;
    unsigned t2;
    unsigned type;

    qint64 rawRemainingTimeNSecs() const Q_DECL_NOTHROW;
};

Q_DECLARE_SHARED(QDeadlineTimer)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeadlineTimer)

#endif // QDEADLINETIMER_H
