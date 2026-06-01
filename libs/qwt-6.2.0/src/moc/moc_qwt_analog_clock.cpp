/****************************************************************************
** Meta object code from reading C++ file 'qwt_analog_clock.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_analog_clock.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_analog_clock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14QwtAnalogClockE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtAnalogClock::qt_create_metaobjectdata<qt_meta_tag_ZN14QwtAnalogClockE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtAnalogClock",
        "setCurrentTime",
        "",
        "setTime"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'setCurrentTime'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setTime'
        QtMocHelpers::SlotData<void(const QTime &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QTime, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtAnalogClock, qt_meta_tag_ZN14QwtAnalogClockE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtAnalogClock::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtDial::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QwtAnalogClockE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QwtAnalogClockE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14QwtAnalogClockE_t>.metaTypes,
    nullptr
} };

void QwtAnalogClock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtAnalogClock *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->setCurrentTime(); break;
        case 1: _t->setTime((*reinterpret_cast<std::add_pointer_t<QTime>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *QwtAnalogClock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtAnalogClock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QwtAnalogClockE_t>.strings))
        return static_cast<void*>(this);
    return QwtDial::qt_metacast(_clname);
}

int QwtAnalogClock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtDial::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
