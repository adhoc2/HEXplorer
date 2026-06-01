/****************************************************************************
** Meta object code from reading C++ file 'qwt_plot_picker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_plot_picker.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_plot_picker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13QwtPlotPickerE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtPlotPicker::qt_create_metaobjectdata<qt_meta_tag_ZN13QwtPlotPickerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtPlotPicker",
        "selected",
        "",
        "QPointF",
        "pos",
        "QRectF",
        "rect",
        "QList<QPointF>",
        "pa",
        "appended",
        "moved"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'selected'
        QtMocHelpers::SignalData<void(const QPointF &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'selected'
        QtMocHelpers::SignalData<void(const QRectF &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'selected'
        QtMocHelpers::SignalData<void(const QVector<QPointF> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'appended'
        QtMocHelpers::SignalData<void(const QPointF &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'moved'
        QtMocHelpers::SignalData<void(const QPointF &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtPlotPicker, qt_meta_tag_ZN13QwtPlotPickerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtPlotPicker::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtPicker::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotPickerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotPickerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13QwtPlotPickerE_t>.metaTypes,
    nullptr
} };

void QwtPlotPicker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtPlotPicker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->selected((*reinterpret_cast<std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 1: _t->selected((*reinterpret_cast<std::add_pointer_t<QRectF>>(_a[1]))); break;
        case 2: _t->selected((*reinterpret_cast<std::add_pointer_t<QList<QPointF>>>(_a[1]))); break;
        case 3: _t->appended((*reinterpret_cast<std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 4: _t->moved((*reinterpret_cast<std::add_pointer_t<QPointF>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtPlotPicker::*)(const QPointF & )>(_a, &QwtPlotPicker::selected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPlotPicker::*)(const QRectF & )>(_a, &QwtPlotPicker::selected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPlotPicker::*)(const QVector<QPointF> & )>(_a, &QwtPlotPicker::selected, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPlotPicker::*)(const QPointF & )>(_a, &QwtPlotPicker::appended, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPlotPicker::*)(const QPointF & )>(_a, &QwtPlotPicker::moved, 4))
            return;
    }
}

const QMetaObject *QwtPlotPicker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtPlotPicker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotPickerE_t>.strings))
        return static_cast<void*>(this);
    return QwtPicker::qt_metacast(_clname);
}

int QwtPlotPicker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPicker::qt_metacall(_c, _id, _a);
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
void QwtPlotPicker::selected(const QPointF & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QwtPlotPicker::selected(const QRectF & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void QwtPlotPicker::selected(const QVector<QPointF> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void QwtPlotPicker::appended(const QPointF & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void QwtPlotPicker::moved(const QPointF & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
