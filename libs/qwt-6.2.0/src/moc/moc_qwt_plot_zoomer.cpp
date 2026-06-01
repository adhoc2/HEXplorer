/****************************************************************************
** Meta object code from reading C++ file 'qwt_plot_zoomer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_plot_zoomer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_plot_zoomer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13QwtPlotZoomerE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtPlotZoomer::qt_create_metaobjectdata<qt_meta_tag_ZN13QwtPlotZoomerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtPlotZoomer",
        "zoomed",
        "",
        "QRectF",
        "rect",
        "moveBy",
        "dx",
        "dy",
        "moveTo",
        "QPointF",
        "zoom",
        "offset"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'zoomed'
        QtMocHelpers::SignalData<void(const QRectF &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'moveBy'
        QtMocHelpers::SlotData<void(double, double)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 6 }, { QMetaType::Double, 7 },
        }}),
        // Slot 'moveTo'
        QtMocHelpers::SlotData<void(const QPointF &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Slot 'zoom'
        QtMocHelpers::SlotData<void(const QRectF &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'zoom'
        QtMocHelpers::SlotData<void(int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtPlotZoomer, qt_meta_tag_ZN13QwtPlotZoomerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtPlotZoomer::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtPlotPicker::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotZoomerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotZoomerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13QwtPlotZoomerE_t>.metaTypes,
    nullptr
} };

void QwtPlotZoomer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtPlotZoomer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->zoomed((*reinterpret_cast<std::add_pointer_t<QRectF>>(_a[1]))); break;
        case 1: _t->moveBy((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 2: _t->moveTo((*reinterpret_cast<std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 3: _t->zoom((*reinterpret_cast<std::add_pointer_t<QRectF>>(_a[1]))); break;
        case 4: _t->zoom((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtPlotZoomer::*)(const QRectF & )>(_a, &QwtPlotZoomer::zoomed, 0))
            return;
    }
}

const QMetaObject *QwtPlotZoomer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtPlotZoomer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotZoomerE_t>.strings))
        return static_cast<void*>(this);
    return QwtPlotPicker::qt_metacast(_clname);
}

int QwtPlotZoomer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlotPicker::qt_metacall(_c, _id, _a);
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
void QwtPlotZoomer::zoomed(const QRectF & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
