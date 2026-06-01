/****************************************************************************
** Meta object code from reading C++ file 'ScrollBar.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ScrollBar.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScrollBar.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9ScrollBarE_t {};
} // unnamed namespace

template <> constexpr inline auto ScrollBar::qt_create_metaobjectdata<qt_meta_tag_ZN9ScrollBarE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScrollBar",
        "sliderMoved",
        "",
        "Qt::Orientation",
        "valueChanged",
        "setBase",
        "min",
        "max",
        "moveSlider",
        "catchValueChanged",
        "value",
        "catchSliderMoved"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sliderMoved'
        QtMocHelpers::SignalData<void(Qt::Orientation, double, double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { QMetaType::Double, 2 }, { QMetaType::Double, 2 },
        }}),
        // Signal 'valueChanged'
        QtMocHelpers::SignalData<void(Qt::Orientation, double, double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { QMetaType::Double, 2 }, { QMetaType::Double, 2 },
        }}),
        // Slot 'setBase'
        QtMocHelpers::SlotData<void(double, double)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 6 }, { QMetaType::Double, 7 },
        }}),
        // Slot 'moveSlider'
        QtMocHelpers::SlotData<void(double, double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 6 }, { QMetaType::Double, 7 },
        }}),
        // Slot 'catchValueChanged'
        QtMocHelpers::SlotData<void(int)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'catchSliderMoved'
        QtMocHelpers::SlotData<void(int)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScrollBar, qt_meta_tag_ZN9ScrollBarE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScrollBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QScrollBar::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9ScrollBarE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9ScrollBarE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9ScrollBarE_t>.metaTypes,
    nullptr
} };

void ScrollBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScrollBar *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sliderMoved((*reinterpret_cast<std::add_pointer_t<Qt::Orientation>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3]))); break;
        case 1: _t->valueChanged((*reinterpret_cast<std::add_pointer_t<Qt::Orientation>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3]))); break;
        case 2: _t->setBase((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 3: _t->moveSlider((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 4: _t->catchValueChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->catchSliderMoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ScrollBar::*)(Qt::Orientation , double , double )>(_a, &ScrollBar::sliderMoved, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScrollBar::*)(Qt::Orientation , double , double )>(_a, &ScrollBar::valueChanged, 1))
            return;
    }
}

const QMetaObject *ScrollBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScrollBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9ScrollBarE_t>.strings))
        return static_cast<void*>(this);
    return QScrollBar::qt_metacast(_clname);
}

int ScrollBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ScrollBar::sliderMoved(Qt::Orientation _t1, double _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void ScrollBar::valueChanged(Qt::Orientation _t1, double _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
