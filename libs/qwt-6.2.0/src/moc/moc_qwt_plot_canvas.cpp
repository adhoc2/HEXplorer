/****************************************************************************
** Meta object code from reading C++ file 'qwt_plot_canvas.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_plot_canvas.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_plot_canvas.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13QwtPlotCanvasE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtPlotCanvas::qt_create_metaobjectdata<qt_meta_tag_ZN13QwtPlotCanvasE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtPlotCanvas",
        "replot",
        "",
        "invalidateBackingStore",
        "borderPath",
        "QPainterPath",
        "QRect",
        "borderRadius"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'replot'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'invalidateBackingStore'
        QtMocHelpers::MethodData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'borderPath'
        QtMocHelpers::MethodData<QPainterPath(const QRect &) const>(4, 2, QMC::AccessPublic, 0x80000000 | 5, {{
            { 0x80000000 | 6, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'borderRadius'
        QtMocHelpers::PropertyData<double>(7, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtPlotCanvas, qt_meta_tag_ZN13QwtPlotCanvasE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtPlotCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotCanvasE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotCanvasE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13QwtPlotCanvasE_t>.metaTypes,
    nullptr
} };

void QwtPlotCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtPlotCanvas *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->replot(); break;
        case 1: _t->invalidateBackingStore(); break;
        case 2: { QPainterPath _r = _t->borderPath((*reinterpret_cast<std::add_pointer_t<QRect>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QPainterPath*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<double*>(_v) = _t->borderRadius(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBorderRadius(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtPlotCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtPlotCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QwtPlotCanvasE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QwtPlotAbstractCanvas"))
        return static_cast< QwtPlotAbstractCanvas*>(this);
    return QFrame::qt_metacast(_clname);
}

int QwtPlotCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
