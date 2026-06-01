/****************************************************************************
** Meta object code from reading C++ file 'qwt_polar_plot.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_polar_plot.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_polar_plot.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12QwtPolarPlotE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtPolarPlot::qt_create_metaobjectdata<qt_meta_tag_ZN12QwtPolarPlotE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtPolarPlot",
        "itemAttached",
        "",
        "QwtPolarItem*",
        "plotItem",
        "on",
        "legendDataChanged",
        "QVariant",
        "itemInfo",
        "QList<QwtLegendData>",
        "data",
        "layoutChanged",
        "replot",
        "autoRefresh",
        "setAzimuthOrigin",
        "plotBackground",
        "QBrush",
        "azimuthOrigin"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'itemAttached'
        QtMocHelpers::SignalData<void(QwtPolarItem *, bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 5 },
        }}),
        // Signal 'legendDataChanged'
        QtMocHelpers::SignalData<void(const QVariant &, const QList<QwtLegendData> &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 }, { 0x80000000 | 9, 10 },
        }}),
        // Signal 'layoutChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'replot'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'autoRefresh'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setAzimuthOrigin'
        QtMocHelpers::SlotData<void(double)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'plotBackground'
        QtMocHelpers::PropertyData<QBrush>(15, 0x80000000 | 16, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'azimuthOrigin'
        QtMocHelpers::PropertyData<double>(17, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtPolarPlot, qt_meta_tag_ZN12QwtPolarPlotE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtPolarPlot::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QwtPolarPlotE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QwtPolarPlotE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12QwtPolarPlotE_t>.metaTypes,
    nullptr
} };

void QwtPolarPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtPolarPlot *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->itemAttached((*reinterpret_cast<std::add_pointer_t<QwtPolarItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 1: _t->legendDataChanged((*reinterpret_cast<std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<QwtLegendData>>>(_a[2]))); break;
        case 2: _t->layoutChanged(); break;
        case 3: _t->replot(); break;
        case 4: _t->autoRefresh(); break;
        case 5: _t->setAzimuthOrigin((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QwtPolarItem* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtPolarPlot::*)(QwtPolarItem * , bool )>(_a, &QwtPolarPlot::itemAttached, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPolarPlot::*)(const QVariant & , const QList<QwtLegendData> & )>(_a, &QwtPolarPlot::legendDataChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPolarPlot::*)()>(_a, &QwtPolarPlot::layoutChanged, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QBrush*>(_v) = _t->plotBackground(); break;
        case 1: *reinterpret_cast<double*>(_v) = _t->azimuthOrigin(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setPlotBackground(*reinterpret_cast<QBrush*>(_v)); break;
        case 1: _t->setAzimuthOrigin(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtPolarPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtPolarPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12QwtPolarPlotE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QwtPolarItemDict"))
        return static_cast< QwtPolarItemDict*>(this);
    return QFrame::qt_metacast(_clname);
}

int QwtPolarPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QwtPolarPlot::itemAttached(QwtPolarItem * _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void QwtPolarPlot::legendDataChanged(const QVariant & _t1, const QList<QwtLegendData> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void QwtPolarPlot::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
