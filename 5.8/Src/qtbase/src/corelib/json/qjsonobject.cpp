/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qjsonarray.h>
#include <qstringlist.h>
#include <qdebug.h>
#include <qvariant.h>
#include "qjson_p.h"
#include "qjsonwriter_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QJsonObject
    \inmodule QtCore
    \ingroup json
    \ingroup shared
    \reentrant
    \since 5.0

    \brief The QJsonObject class encapsulates a JSON object.

    A JSON object is a list of key value pairs, where the keys are unique strings
    and the values are represented by a QJsonValue.

    A QJsonObject can be converted to and from a QVariantMap. You can query the
    number of (key, value) pairs with size(), insert(), and remove() entries from it
    and iterate over its content using the standard C++ iterator pattern.

    QJsonObject is an implicitly shared class, and shares the data with the document
    it has been created from as long as it is not being modified.

    You can convert the object to and from text based JSON through QJsonDocument.

    \sa {JSON Support in Qt}, {JSON Save Game Example}
*/

/*!
    \typedef QJsonObject::Iterator

    Qt-style synonym for QJsonObject::iterator.
*/

/*!
    \typedef QJsonObject::ConstIterator

    Qt-style synonym for QJsonObject::const_iterator.
*/

/*!
    \typedef QJsonObject::key_type

    Typedef for QString. Provided for STL compatibility.
*/

/*!
    \typedef QJsonObject::mapped_type

    Typedef for QJsonValue. Provided for STL compatibility.
*/

/*!
    \typedef QJsonObject::size_type

    Typedef for int. Provided for STL compatibility.
*/


/*!
    Constructs an empty JSON object.

    \sa isEmpty()
 */
QJsonObject::QJsonObject()
    : d(0), o(0)
{
}

/*!
    \fn QJsonObject::QJsonObject(std::initializer_list<QPair<QString, QJsonValue> > args)
    \since 5.4
    Constructs a QJsonObject instance initialized from \a args initialization list.
    For example:
    \code
    QJsonObject object
    {
        {"property1", 1},
        {"property2", 2}
    };
    \endcode
*/

/*!
    \internal
 */
QJsonObject::QJsonObject(QJsonPrivate::Data *data, QJsonPrivate::Object *object)
    : d(data), o(object)
{
    Q_ASSERT(d);
    Q_ASSERT(o);
    d->ref.ref();
}

/*!
    This method replaces part of the QJsonObject(std::initializer_list<QPair<QString, QJsonValue>> args) body.
    The constructor needs to be inline, but we do not want to leak implementation details
    of this class.
    \note this method is called for an uninitialized object
    \internal
 */

void QJsonObject::initialize()
{
    d = 0;
    o = 0;
}

/*!
    Destroys the object.
 */
QJsonObject::~QJsonObject()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Creates a copy of \a other.

    Since QJsonObject is implicitly shared, the copy is shallow
    as long as the object does not get modified.
 */
QJsonObject::QJsonObject(const QJsonObject &other)
{
    d = other.d;
    o = other.o;
    if (d)
        d->ref.ref();
}

/*!
    Assigns \a other to this object.
 */
QJsonObject &QJsonObject::operator =(const QJsonObject &other)
{
    if (d != other.d) {
        if (d && !d->ref.deref())
            delete d;
        d = other.d;
        if (d)
            d->ref.ref();
    }
    o = other.o;

    return *this;
}

/*!
    Converts the variant map \a map to a QJsonObject.

    The keys in \a map will be used as the keys in the JSON object,
    and the QVariant values will be converted to JSON values.

    \sa fromVariantHash(), toVariantMap(), QJsonValue::fromVariant()
 */
QJsonObject QJsonObject::fromVariantMap(const QVariantMap &map)
{
    QJsonObject object;
    if (map.isEmpty())
        return object;

    object.detach2(1024);

    QVector<QJsonPrivate::offset> offsets;
    QJsonPrivate::offset currentOffset;
    currentOffset = sizeof(QJsonPrivate::Base);

    // the map is already sorted, so we can simply append one entry after the other and
    // write the offset table at the end
    for (QVariantMap::const_iterator it = map.constBegin(); it != map.constEnd(); ++it) {
        QString key = it.key();
        QJsonValue val = QJsonValue::fromVariant(it.value());

        bool latinOrIntValue;
        int valueSize = QJsonPrivate::Value::requiredStorage(val, &latinOrIntValue);

        bool latinKey = QJsonPrivate::useCompressed(key);
        int valueOffset = sizeof(QJsonPrivate::Entry) + QJsonPrivate::qStringSize(key, latinKey);
        int requiredSize = valueOffset + valueSize;

        if (!object.detach2(requiredSize + sizeof(QJsonPrivate::offset))) // offset for the new index entry
            return QJsonObject();

        QJsonPrivate::Entry *e = reinterpret_cast<QJsonPrivate::Entry *>(reinterpret_cast<char *>(object.o) + currentOffset);
        e->value.type = val.t;
        e->value.latinKey = latinKey;
        e->value.latinOrIntValue = latinOrIntValue;
        e->value.value = QJsonPrivate::Value::valueToStore(val, (char *)e - (char *)object.o + valueOffset);
        QJsonPrivate::copyString((char *)(e + 1), key, latinKey);
        if (valueSize)
            QJsonPrivate::Value::copyData(val, (char *)e + valueOffset, latinOrIntValue);

        offsets << currentOffset;
        currentOffset += requiredSize;
        object.o->size = currentOffset;
    }

    // write table
    object.o->tableOffset = currentOffset;
    if (!object.detach2(sizeof(QJsonPrivate::offset)*offsets.size()))
        return QJsonObject();
    memcpy(object.o->table(), offsets.constData(), offsets.size()*sizeof(uint));
    object.o->length = offsets.size();
    object.o->size = currentOffset + sizeof(QJsonPrivate::offset)*offsets.size();

    return object;
}

/*!
    Converts this object to a QVariantMap.

    Returns the created map.

    \sa toVariantHash()
 */
QVariantMap QJsonObject::toVariantMap() const
{
    QVariantMap map;
    if (o) {
        for (uint i = 0; i < o->length; ++i) {
            QJsonPrivate::Entry *e = o->entryAt(i);
            map.insert(e->key(), QJsonValue(d, o, e->value).toVariant());
        }
    }
    return map;
}

/*!
    Converts the variant hash \a hash to a QJsonObject.
    \since 5.5

    The keys in \a hash will be used as the keys in the JSON object,
    and the QVariant values will be converted to JSON values.

    \sa fromVariantMap(), toVariantHash(), QJsonValue::fromVariant()
 */
QJsonObject QJsonObject::fromVariantHash(const QVariantHash &hash)
{
    // ### this is implemented the trivial way, not the most efficient way

    QJsonObject object;
    for (QVariantHash::const_iterator it = hash.constBegin(); it != hash.constEnd(); ++it)
        object.insert(it.key(), QJsonValue::fromVariant(it.value()));
    return object;
}

/*!
    Converts this object to a QVariantHash.
    \since 5.5

    Returns the created hash.

    \sa toVariantMap()
 */
QVariantHash QJsonObject::toVariantHash() const
{
    QVariantHash hash;
    if (o) {
        hash.reserve(o->length);
        for (uint i = 0; i < o->length; ++i) {
            QJsonPrivate::Entry *e = o->entryAt(i);
            hash.insert(e->key(), QJsonValue(d, o, e->value).toVariant());
        }
    }
    return hash;
}

/*!
    Returns a list of all keys in this object.

    The list is sorted lexographically.
 */
QStringList QJsonObject::keys() const
{
    QStringList keys;
    if (o) {
        keys.reserve(o->length);
        for (uint i = 0; i < o->length; ++i) {
            QJsonPrivate::Entry *e = o->entryAt(i);
            keys.append(e->key());
        }
    }
    return keys;
}

/*!
    Returns the number of (key, value) pairs stored in the object.
 */
int QJsonObject::size() const
{
    if (!d)
        return 0;

    return o->length;
}

/*!
    Returns \c true if the object is empty. This is the same as size() == 0.

    \sa size()
 */
bool QJsonObject::isEmpty() const
{
    if (!d)
        return true;

    return !o->length;
}

/*!
    Returns a QJsonValue representing the value for the key \a key.

    The returned QJsonValue is QJsonValue::Undefined if the key does not exist.

    \sa QJsonValue, QJsonValue::isUndefined()
 */
QJsonValue QJsonObject::value(const QString &key) const
{
    if (!d)
        return QJsonValue(QJsonValue::Undefined);

    bool keyExists;
    int i = o->indexOf(key, &keyExists);
    if (!keyExists)
        return QJsonValue(QJsonValue::Undefined);
    return QJsonValue(d, o, o->entryAt(i)->value);
}

/*!
    \overload
    \since 5.7
*/
QJsonValue QJsonObject::value(QLatin1String key) const
{
    if (!d)
        return QJsonValue(QJsonValue::Undefined);

    bool keyExists;
    int i = o->indexOf(key, &keyExists);
    if (!keyExists)
        return QJsonValue(QJsonValue::Undefined);
    return QJsonValue(d, o, o->entryAt(i)->value);
}

/*!
    Returns a QJsonValue representing the value for the key \a key.

    This does the same as value().

    The returned QJsonValue is QJsonValue::Undefined if the key does not exist.

    \sa value(), QJsonValue, QJsonValue::isUndefined()
 */
QJsonValue QJsonObject::operator [](const QString &key) const
{
    return value(key);
}

/*!
    \fn QJsonValue QJsonObject::operator [](QLatin1String key) const

    \overload
    \since 5.7
*/

/*!
    Returns a reference to the value for \a key.

    The return value is of type QJsonValueRef, a helper class for QJsonArray
    and QJsonObject. When you get an object of type QJsonValueRef, you can
    use it as if it were a reference to a QJsonValue. If you assign to it,
    the assignment will apply to the element in the QJsonArray or QJsonObject
    from which you got the reference.

    \sa value()
 */
QJsonValueRef QJsonObject::operator [](const QString &key)
{
    // ### somewhat inefficient, as we lookup the key twice if it doesn't yet exist
    bool keyExists = false;
    int index = o ? o->indexOf(key, &keyExists) : -1;
    if (!keyExists) {
        iterator i = insert(key, QJsonValue());
        index = i.i;
    }
    return QJsonValueRef(this, index);
}

/*!
    \overload
    \since 5.7
*/
QJsonValueRef QJsonObject::operator [](QLatin1String key)
{
    // ### optimize me
    return operator[](QString(key));
}

/*!
    Inserts a new item with the key \a key and a value of \a value.

    If there is already an item with the key \a key, then that item's value
    is replaced with \a value.

    Returns an iterator pointing to the inserted item.

    If the value is QJsonValue::Undefined, it will cause the key to get removed
    from the object. The returned iterator will then point to end().

    \sa remove(), take(), QJsonObject::iterator, end()
 */
QJsonObject::iterator QJsonObject::insert(const QString &key, const QJsonValue &value)
{
    if (value.t == QJsonValue::Undefined) {
        remove(key);
        return end();
    }
    QJsonValue val = value;

    bool latinOrIntValue;
    int valueSize = QJsonPrivate::Value::requiredStorage(val, &latinOrIntValue);

    bool latinKey = QJsonPrivate::useCompressed(key);
    int valueOffset = sizeof(QJsonPrivate::Entry) + QJsonPrivate::qStringSize(key, latinKey);
    int requiredSize = valueOffset + valueSize;

    if (!detach2(requiredSize + sizeof(QJsonPrivate::offset))) // offset for the new index entry
        return iterator();

    if (!o->length)
        o->tableOffset = sizeof(QJsonPrivate::Object);

    bool keyExists = false;
    int pos = o->indexOf(key, &keyExists);
    if (keyExists)
        ++d->compactionCounter;

    uint off = o->reserveSpace(requiredSize, pos, 1, keyExists);
    if (!off)
        return end();

    QJsonPrivate::Entry *e = o->entryAt(pos);
    e->value.type = val.t;
    e->value.latinKey = latinKey;
    e->value.latinOrIntValue = latinOrIntValue;
    e->value.value = QJsonPrivate::Value::valueToStore(val, (char *)e - (char *)o + valueOffset);
    QJsonPrivate::copyString((char *)(e + 1), key, latinKey);
    if (valueSize)
        QJsonPrivate::Value::copyData(val, (char *)e + valueOffset, latinOrIntValue);

    if (d->compactionCounter > 32u && d->compactionCounter >= unsigned(o->length) / 2u)
        compact();

    return iterator(this, pos);
}

/*!
    Removes \a key from the object.

    \sa insert(), take()
 */
void QJsonObject::remove(const QString &key)
{
    if (!d)
        return;

    bool keyExists;
    int index = o->indexOf(key, &keyExists);
    if (!keyExists)
        return;

    detach2();
    o->removeItems(index, 1);
    ++d->compactionCounter;
    if (d->compactionCounter > 32u && d->compactionCounter >= unsigned(o->length) / 2u)
        compact();
}

/*!
    Removes \a key from the object.

    Returns a QJsonValue containing the value referenced by \a key.
    If \a key was not contained in the object, the returned QJsonValue
    is QJsonValue::Undefined.

    \sa insert(), remove(), QJsonValue
 */
QJsonValue QJsonObject::take(const QString &key)
{
    if (!o)
        return QJsonValue(QJsonValue::Undefined);

    bool keyExists;
    int index = o->indexOf(key, &keyExists);
    if (!keyExists)
        return QJsonValue(QJsonValue::Undefined);

    QJsonValue v(d, o, o->entryAt(index)->value);
    detach2();
    o->removeItems(index, 1);
    ++d->compactionCounter;
    if (d->compactionCounter > 32u && d->compactionCounter >= unsigned(o->length) / 2u)
        compact();

    return v;
}

/*!
    Returns \c true if the object contains key \a key.

    \sa insert(), remove(), take()
 */
bool QJsonObject::contains(const QString &key) const
{
    if (!o)
        return false;

    bool keyExists;
    o->indexOf(key, &keyExists);
    return keyExists;
}

/*!
    \overload
    \since 5.7
*/
bool QJsonObject::contains(QLatin1String key) const
{
    if (!o)
        return false;

    bool keyExists;
    o->indexOf(key, &keyExists);
    return keyExists;
}

/*!
    Returns \c true if \a other is equal to this object.
 */
bool QJsonObject::operator==(const QJsonObject &other) const
{
    if (o == other.o)
        return true;

    if (!o)
        return !other.o->length;
    if (!other.o)
        return !o->length;
    if (o->length != other.o->length)
        return false;

    for (uint i = 0; i < o->length; ++i) {
        QJsonPrivate::Entry *e = o->entryAt(i);
        QJsonValue v(d, o, e->value);
        if (other.value(e->key()) != v)
            return false;
    }

    return true;
}

/*!
    Returns \c true if \a other is not equal to this object.
 */
bool QJsonObject::operator!=(const QJsonObject &other) const
{
    return !(*this == other);
}

/*!
    Removes the (key, value) pair pointed to by the iterator \a it
    from the map, and returns an iterator to the next item in the
    map.

    \sa remove()
 */
QJsonObject::iterator QJsonObject::erase(QJsonObject::iterator it)
{
    Q_ASSERT(d && d->ref.load() == 1);
    if (it.o != this || it.i < 0 || it.i >= (int)o->length)
        return iterator(this, o->length);

    int index = it.i;

    o->removeItems(index, 1);
    ++d->compactionCounter;
    if (d->compactionCounter > 32u && d->compactionCounter >= unsigned(o->length) / 2u)
        compact();

    // iterator hasn't changed
    return it;
}

/*!
    Returns an iterator pointing to the item with key \a key in the
    map.

    If the map contains no item with key \a key, the function
    returns end().
 */
QJsonObject::iterator QJsonObject::find(const QString &key)
{
    bool keyExists = false;
    int index = o ? o->indexOf(key, &keyExists) : 0;
    if (!keyExists)
        return end();
    detach2();
    return iterator(this, index);
}

/*!
    \overload
    \since 5.7
*/
QJsonObject::iterator QJsonObject::find(QLatin1String key)
{
    bool keyExists = false;
    int index = o ? o->indexOf(key, &keyExists) : 0;
    if (!keyExists)
        return end();
    detach2();
    return iterator(this, index);
}

/*! \fn QJsonObject::const_iterator QJsonObject::find(const QString &key) const

    \overload
*/

/*! \fn QJsonObject::const_iterator QJsonObject::find(QLatin1String key) const

    \overload
    \since 5.7
*/

/*!
    Returns a const iterator pointing to the item with key \a key in the
    map.

    If the map contains no item with key \a key, the function
    returns constEnd().
 */
QJsonObject::const_iterator QJsonObject::constFind(const QString &key) const
{
    bool keyExists = false;
    int index = o ? o->indexOf(key, &keyExists) : 0;
    if (!keyExists)
        return end();
    return const_iterator(this, index);
}

/*!
    \overload
    \since 5.7
*/
QJsonObject::const_iterator QJsonObject::constFind(QLatin1String key) const
{
    bool keyExists = false;
    int index = o ? o->indexOf(key, &keyExists) : 0;
    if (!keyExists)
        return end();
    return const_iterator(this, index);
}

/*! \fn int QJsonObject::count() const

    \overload

    Same as size().
*/

/*! \fn int QJsonObject::length() const

    \overload

    Same as size().
*/

/*! \fn QJsonObject::iterator QJsonObject::begin()

    Returns an \l{STL-style iterators}{STL-style iterator} pointing to the first item in
    the object.

    \sa constBegin(), end()
*/

/*! \fn QJsonObject::const_iterator QJsonObject::begin() const

    \overload
*/

/*! \fn QJsonObject::const_iterator QJsonObject::constBegin() const

    Returns a const \l{STL-style iterators}{STL-style iterator} pointing to the first item
    in the object.

    \sa begin(), constEnd()
*/

/*! \fn QJsonObject::iterator QJsonObject::end()

    Returns an \l{STL-style iterators}{STL-style iterator} pointing to the imaginary item
    after the last item in the object.

    \sa begin(), constEnd()
*/

/*! \fn QJsonObject::const_iterator QJsonObject::end() const

    \overload
*/

/*! \fn QJsonObject::const_iterator QJsonObject::constEnd() const

    Returns a const \l{STL-style iterators}{STL-style iterator} pointing to the imaginary
    item after the last item in the object.

    \sa constBegin(), end()
*/

/*!
    \fn bool QJsonObject::empty() const

    This function is provided for STL compatibility. It is equivalent
    to isEmpty(), returning \c true if the object is empty; otherwise
    returning \c false.
*/

/*! \class QJsonObject::iterator
    \inmodule QtCore
    \ingroup json
    \reentrant
    \since 5.0

    \brief The QJsonObject::iterator class provides an STL-style non-const iterator for QJsonObject.

    QJsonObject::iterator allows you to iterate over a QJsonObject
    and to modify the value (but not the key) stored under
    a particular key. If you want to iterate over a const QJsonObject, you
    should use QJsonObject::const_iterator. It is generally good practice to
    use QJsonObject::const_iterator on a non-const QJsonObject as well, unless you
    need to change the QJsonObject through the iterator. Const iterators are
    slightly faster, and improve code readability.

    The default QJsonObject::iterator constructor creates an uninitialized
    iterator. You must initialize it using a QJsonObject function like
    QJsonObject::begin(), QJsonObject::end(), or QJsonObject::find() before you can
    start iterating.

    Multiple iterators can be used on the same object. Existing iterators will however
    become dangling once the object gets modified.

    \sa QJsonObject::const_iterator, {JSON Support in Qt}, {JSON Save Game Example}
*/

/*! \typedef QJsonObject::iterator::difference_type

    \internal
*/

/*! \typedef QJsonObject::iterator::iterator_category

    A synonym for \e {std::random_access_iterator_tag} indicating
    this iterator is a random-access iterator.

    \note In Qt versions before 5.6, this was set by mistake to
    \e {std::bidirectional_iterator_tag}.
*/

/*! \typedef QJsonObject::iterator::reference

    \internal
*/

/*! \typedef QJsonObject::iterator::value_type

    \internal
*/

/*! \typedef QJsonObject::iterator::pointer

    \internal
*/

/*! \fn QJsonObject::iterator::iterator()

    Constructs an uninitialized iterator.

    Functions like key(), value(), and operator++() must not be
    called on an uninitialized iterator. Use operator=() to assign a
    value to it before using it.

    \sa QJsonObject::begin(), QJsonObject::end()
*/

/*! \fn QJsonObject::iterator::iterator(QJsonObject *obj, int index)
    \internal
*/

/*! \fn QString QJsonObject::iterator::key() const

    Returns the current item's key.

    There is no direct way of changing an item's key through an
    iterator, although it can be done by calling QJsonObject::erase()
    followed by QJsonObject::insert().

    \sa value()
*/

/*! \fn QJsonValueRef QJsonObject::iterator::value() const

    Returns a modifiable reference to the current item's value.

    You can change the value of an item by using value() on
    the left side of an assignment.

    The return value is of type QJsonValueRef, a helper class for QJsonArray
    and QJsonObject. When you get an object of type QJsonValueRef, you can
    use it as if it were a reference to a QJsonValue. If you assign to it,
    the assignment will apply to the element in the QJsonArray or QJsonObject
    from which you got the reference.

    \sa key(), operator*()
*/

/*! \fn QJsonValueRef QJsonObject::iterator::operator*() const

    Returns a modifiable reference to the current item's value.

    Same as value().

    The return value is of type QJsonValueRef, a helper class for QJsonArray
    and QJsonObject. When you get an object of type QJsonValueRef, you can
    use it as if it were a reference to a QJsonValue. If you assign to it,
    the assignment will apply to the element in the QJsonArray or QJsonObject
    from which you got the reference.

    \sa key()
*/

/*! \fn QJsonValueRef *QJsonObject::iterator::operator->() const

    Returns a pointer to a modifiable reference to the current item.
*/

/*!
    \fn bool QJsonObject::iterator::operator==(const iterator &other) const
    \fn bool QJsonObject::iterator::operator==(const const_iterator &other) const

    Returns \c true if \a other points to the same item as this
    iterator; otherwise returns \c false.

    \sa operator!=()
*/

/*!
    \fn bool QJsonObject::iterator::operator!=(const iterator &other) const
    \fn bool QJsonObject::iterator::operator!=(const const_iterator &other) const

    Returns \c true if \a other points to a different item than this
    iterator; otherwise returns \c false.

    \sa operator==()
*/

/*! \fn QJsonObject::iterator QJsonObject::iterator::operator++()

    The prefix ++ operator, \c{++i}, advances the iterator to the
    next item in the object and returns an iterator to the new current
    item.

    Calling this function on QJsonObject::end() leads to undefined results.

    \sa operator--()
*/

/*! \fn QJsonObject::iterator QJsonObject::iterator::operator++(int)

    \overload

    The postfix ++ operator, \c{i++}, advances the iterator to the
    next item in the object and returns an iterator to the previously
    current item.
*/

/*! \fn QJsonObject::iterator QJsonObject::iterator::operator--()

    The prefix -- operator, \c{--i}, makes the preceding item
    current and returns an iterator pointing to the new current item.

    Calling this function on QJsonObject::begin() leads to undefined
    results.

    \sa operator++()
*/

/*! \fn QJsonObject::iterator QJsonObject::iterator::operator--(int)

    \overload

    The postfix -- operator, \c{i--}, makes the preceding item
    current and returns an iterator pointing to the previously
    current item.
*/

/*! \fn QJsonObject::iterator QJsonObject::iterator::operator+(int j) const

    Returns an iterator to the item at \a j positions forward from
    this iterator. If \a j is negative, the iterator goes backward.

    \sa operator-()

*/

/*! \fn QJsonObject::iterator QJsonObject::iterator::operator-(int j) const

    Returns an iterator to the item at \a j positions backward from
    this iterator. If \a j is negative, the iterator goes forward.

    \sa operator+()
*/

/*! \fn QJsonObject::iterator &QJsonObject::iterator::operator+=(int j)

    Advances the iterator by \a j items. If \a j is negative, the
    iterator goes backward.

    \sa operator-=(), operator+()
*/

/*! \fn QJsonObject::iterator &QJsonObject::iterator::operator-=(int j)

    Makes the iterator go back by \a j items. If \a j is negative,
    the iterator goes forward.

    \sa operator+=(), operator-()
*/

/*!
    \class QJsonObject::const_iterator
    \inmodule QtCore
    \ingroup json
    \since 5.0
    \brief The QJsonObject::const_iterator class provides an STL-style const iterator for QJsonObject.

    QJsonObject::const_iterator allows you to iterate over a QJsonObject.
    If you want to modify the QJsonObject as you iterate
    over it, you must use QJsonObject::iterator instead. It is generally
    good practice to use QJsonObject::const_iterator on a non-const QJsonObject as
    well, unless you need to change the QJsonObject through the iterator.
    Const iterators are slightly faster and improve code
    readability.

    The default QJsonObject::const_iterator constructor creates an
    uninitialized iterator. You must initialize it using a QJsonObject
    function like QJsonObject::constBegin(), QJsonObject::constEnd(), or
    QJsonObject::find() before you can start iterating.

    Multiple iterators can be used on the same object. Existing iterators
    will however become dangling if the object gets modified.

    \sa QJsonObject::iterator, {JSON Support in Qt}, {JSON Save Game Example}
*/

/*! \typedef QJsonObject::const_iterator::difference_type

    \internal
*/

/*! \typedef QJsonObject::const_iterator::iterator_category

    A synonym for \e {std::random_access_iterator_tag} indicating
    this iterator is a random-access iterator.

    \note In Qt versions before 5.6, this was set by mistake to
    \e {std::bidirectional_iterator_tag}.
*/

/*! \typedef QJsonObject::const_iterator::reference

    \internal
*/

/*! \typedef QJsonObject::const_iterator::value_type

    \internal
*/

/*! \typedef QJsonObject::const_iterator::pointer

    \internal
*/

/*! \fn QJsonObject::const_iterator::const_iterator()

    Constructs an uninitialized iterator.

    Functions like key(), value(), and operator++() must not be
    called on an uninitialized iterator. Use operator=() to assign a
    value to it before using it.

    \sa QJsonObject::constBegin(), QJsonObject::constEnd()
*/

/*! \fn QJsonObject::const_iterator::const_iterator(const QJsonObject *obj, int index)
    \internal
*/

/*! \fn QJsonObject::const_iterator::const_iterator(const iterator &other)

    Constructs a copy of \a other.
*/

/*! \fn QString QJsonObject::const_iterator::key() const

    Returns the current item's key.

    \sa value()
*/

/*! \fn QJsonValue QJsonObject::const_iterator::value() const

    Returns the current item's value.

    \sa key(), operator*()
*/

/*! \fn QJsonValue QJsonObject::const_iterator::operator*() const

    Returns the current item's value.

    Same as value().

    \sa key()
*/

/*! \fn QJsonValue *QJsonObject::const_iterator::operator->() const

    Returns a pointer to the current item.
*/

/*! \fn bool QJsonObject::const_iterator::operator==(const const_iterator &other) const
    \fn bool QJsonObject::const_iterator::operator==(const iterator &other) const

    Returns \c true if \a other points to the same item as this
    iterator; otherwise returns \c false.

    \sa operator!=()
*/

/*! \fn bool QJsonObject::const_iterator::operator!=(const const_iterator &other) const
    \fn bool QJsonObject::const_iterator::operator!=(const iterator &other) const

    Returns \c true if \a other points to a different item than this
    iterator; otherwise returns \c false.

    \sa operator==()
*/

/*! \fn QJsonObject::const_iterator QJsonObject::const_iterator::operator++()

    The prefix ++ operator, \c{++i}, advances the iterator to the
    next item in the object and returns an iterator to the new current
    item.

    Calling this function on QJsonObject::end() leads to undefined results.

    \sa operator--()
*/

/*! \fn QJsonObject::const_iterator QJsonObject::const_iterator::operator++(int)

    \overload

    The postfix ++ operator, \c{i++}, advances the iterator to the
    next item in the object and returns an iterator to the previously
    current item.
*/

/*! \fn QJsonObject::const_iterator &QJsonObject::const_iterator::operator--()

    The prefix -- operator, \c{--i}, makes the preceding item
    current and returns an iterator pointing to the new current item.

    Calling this function on QJsonObject::begin() leads to undefined
    results.

    \sa operator++()
*/

/*! \fn QJsonObject::const_iterator QJsonObject::const_iterator::operator--(int)

    \overload

    The postfix -- operator, \c{i--}, makes the preceding item
    current and returns an iterator pointing to the previously
    current item.
*/

/*! \fn QJsonObject::const_iterator QJsonObject::const_iterator::operator+(int j) const

    Returns an iterator to the item at \a j positions forward from
    this iterator. If \a j is negative, the iterator goes backward.

    This operation can be slow for large \a j values.

    \sa operator-()
*/

/*! \fn QJsonObject::const_iterator QJsonObject::const_iterator::operator-(int j) const

    Returns an iterator to the item at \a j positions backward from
    this iterator. If \a j is negative, the iterator goes forward.

    This operation can be slow for large \a j values.

    \sa operator+()
*/

/*! \fn QJsonObject::const_iterator &QJsonObject::const_iterator::operator+=(int j)

    Advances the iterator by \a j items. If \a j is negative, the
    iterator goes backward.

    This operation can be slow for large \a j values.

    \sa operator-=(), operator+()
*/

/*! \fn QJsonObject::const_iterator &QJsonObject::const_iterator::operator-=(int j)

    Makes the iterator go back by \a j items. If \a j is negative,
    the iterator goes forward.

    This operation can be slow for large \a j values.

    \sa operator+=(), operator-()
*/


/*!
    \internal
 */
void QJsonObject::detach(uint reserve)
{
    Q_UNUSED(reserve)
    Q_ASSERT(!reserve);
    detach2(reserve);
}

bool QJsonObject::detach2(uint reserve)
{
    if (!d) {
        if (reserve >= QJsonPrivate::Value::MaxSize) {
            qWarning("QJson: Document too large to store in data structure");
            return false;
        }
        d = new QJsonPrivate::Data(reserve, QJsonValue::Object);
        o = static_cast<QJsonPrivate::Object *>(d->header->root());
        d->ref.ref();
        return true;
    }
    if (reserve == 0 && d->ref.load() == 1)
        return true;

    QJsonPrivate::Data *x = d->clone(o, reserve);
    if (!x)
        return false;
    x->ref.ref();
    if (!d->ref.deref())
        delete d;
    d = x;
    o = static_cast<QJsonPrivate::Object *>(d->header->root());
    return true;
}

/*!
    \internal
 */
void QJsonObject::compact()
{
    if (!d || !d->compactionCounter)
        return;

    detach2();
    d->compact();
    o = static_cast<QJsonPrivate::Object *>(d->header->root());
}

/*!
    \internal
 */
QString QJsonObject::keyAt(int i) const
{
    Q_ASSERT(o && i >= 0 && i < (int)o->length);

    QJsonPrivate::Entry *e = o->entryAt(i);
    return e->key();
}

/*!
    \internal
 */
QJsonValue QJsonObject::valueAt(int i) const
{
    if (!o || i < 0 || i >= (int)o->length)
        return QJsonValue(QJsonValue::Undefined);

    QJsonPrivate::Entry *e = o->entryAt(i);
    return QJsonValue(d, o, e->value);
}

/*!
    \internal
 */
void QJsonObject::setValueAt(int i, const QJsonValue &val)
{
    Q_ASSERT(o && i >= 0 && i < (int)o->length);

    QJsonPrivate::Entry *e = o->entryAt(i);
    insert(e->key(), val);
}

#if !defined(QT_NO_DEBUG_STREAM) && !defined(QT_JSON_READONLY)
QDebug operator<<(QDebug dbg, const QJsonObject &o)
{
    QDebugStateSaver saver(dbg);
    if (!o.o) {
        dbg << "QJsonObject()";
        return dbg;
    }
    QByteArray json;
    QJsonPrivate::Writer::objectToJson(o.o, json, 0, true);
    dbg.nospace() << "QJsonObject("
                  << json.constData() // print as utf-8 string without extra quotation marks
                  << ")";
    return dbg;
}
#endif

QT_END_NAMESPACE
