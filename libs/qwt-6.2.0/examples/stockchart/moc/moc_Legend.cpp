/****************************************************************************
** Meta object code from reading C++ file 'Legend.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Legend.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Legend.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6LegendE_t {};
} // unnamed namespace

template <> constexpr inline auto Legend::qt_create_metaobjectdata<qt_meta_tag_ZN6LegendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Legend",
        "checked",
        "",
        "QwtPlotItem*",
        "plotItem",
        "on",
        "index",
        "updateLegend",
        "QVariant",
        "QList<QwtLegendData>",
        "handleClick",
        "QModelIndex"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'checked'
        QtMocHelpers::SignalData<void(QwtPlotItem *, bool, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 5 }, { QMetaType::Int, 6 },
        }}),
        // Slot 'updateLegend'
        QtMocHelpers::SlotData<void(const QVariant &, const QList<QwtLegendData> &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 2 }, { 0x80000000 | 9, 2 },
        }}),
        // Slot 'handleClick'
        QtMocHelpers::SlotData<void(const QModelIndex &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 11, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Legend, qt_meta_tag_ZN6LegendE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Legend::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractLegend::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6LegendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6LegendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6LegendE_t>.metaTypes,
    nullptr
} };

void Legend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Legend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->checked((*reinterpret_cast<std::add_pointer_t<QwtPlotItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 1: _t->updateLegend((*reinterpret_cast<std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<QwtLegendData>>>(_a[2]))); break;
        case 2: _t->handleClick((*reinterpret_cast<std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Legend::*)(QwtPlotItem * , bool , int )>(_a, &Legend::checked, 0))
            return;
    }
}

const QMetaObject *Legend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Legend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6LegendE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractLegend::qt_metacast(_clname);
}

int Legend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractLegend::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Legend::checked(QwtPlotItem * _t1, bool _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
