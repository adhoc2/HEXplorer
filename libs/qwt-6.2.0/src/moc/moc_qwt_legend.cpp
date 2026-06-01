/****************************************************************************
** Meta object code from reading C++ file 'qwt_legend.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_legend.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_legend.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9QwtLegendE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtLegend::qt_create_metaobjectdata<qt_meta_tag_ZN9QwtLegendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtLegend",
        "clicked",
        "",
        "QVariant",
        "itemInfo",
        "index",
        "checked",
        "on",
        "updateLegend",
        "QList<QwtLegendData>",
        "itemClicked",
        "itemChecked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'clicked'
        QtMocHelpers::SignalData<void(const QVariant &, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Int, 5 },
        }}),
        // Signal 'checked'
        QtMocHelpers::SignalData<void(const QVariant &, bool, int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 7 }, { QMetaType::Int, 5 },
        }}),
        // Slot 'updateLegend'
        QtMocHelpers::SlotData<void(const QVariant &, const QList<QwtLegendData> &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 9, 2 },
        }}),
        // Slot 'itemClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessProtected, QMetaType::Void),
        // Slot 'itemChecked'
        QtMocHelpers::SlotData<void(bool)>(11, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtLegend, qt_meta_tag_ZN9QwtLegendE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtLegend::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractLegend::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtLegendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtLegendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9QwtLegendE_t>.metaTypes,
    nullptr
} };

void QwtLegend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtLegend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast<std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->checked((*reinterpret_cast<std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 2: _t->updateLegend((*reinterpret_cast<std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<QwtLegendData>>>(_a[2]))); break;
        case 3: _t->itemClicked(); break;
        case 4: _t->itemChecked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtLegend::*)(const QVariant & , int )>(_a, &QwtLegend::clicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtLegend::*)(const QVariant & , bool , int )>(_a, &QwtLegend::checked, 1))
            return;
    }
}

const QMetaObject *QwtLegend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtLegend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtLegendE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractLegend::qt_metacast(_clname);
}

int QwtLegend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractLegend::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QwtLegend::clicked(const QVariant & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void QwtLegend::checked(const QVariant & _t1, bool _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
