/****************************************************************************
** Meta object code from reading C++ file 'qwt_dial.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_dial.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_dial.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN7QwtDialE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtDial::qt_create_metaobjectdata<qt_meta_tag_ZN7QwtDialE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtDial",
        "lineWidth",
        "frameShadow",
        "Shadow",
        "mode",
        "Mode",
        "origin",
        "minScaleArc",
        "maxScaleArc",
        "Plain",
        "Raised",
        "Sunken",
        "RotateNeedle",
        "RotateScale"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
        // property 'lineWidth'
        QtMocHelpers::PropertyData<int>(1, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'frameShadow'
        QtMocHelpers::PropertyData<enum Shadow>(2, 0x80000000 | 3, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'mode'
        QtMocHelpers::PropertyData<enum Mode>(4, 0x80000000 | 5, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'origin'
        QtMocHelpers::PropertyData<double>(6, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'minScaleArc'
        QtMocHelpers::PropertyData<double>(7, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'maxScaleArc'
        QtMocHelpers::PropertyData<double>(8, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Shadow'
        QtMocHelpers::EnumData<enum Shadow>(3, 3, QMC::EnumFlags{}).add({
            {    9, Shadow::Plain },
            {   10, Shadow::Raised },
            {   11, Shadow::Sunken },
        }),
        // enum 'Mode'
        QtMocHelpers::EnumData<enum Mode>(5, 5, QMC::EnumFlags{}).add({
            {   12, Mode::RotateNeedle },
            {   13, Mode::RotateScale },
        }),
    };
    return QtMocHelpers::metaObjectData<QwtDial, qt_meta_tag_ZN7QwtDialE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtDial::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractSlider::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QwtDialE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QwtDialE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7QwtDialE_t>.metaTypes,
    nullptr
} };

void QwtDial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtDial *>(_o);
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->lineWidth(); break;
        case 1: *reinterpret_cast<enum Shadow*>(_v) = _t->frameShadow(); break;
        case 2: *reinterpret_cast<enum Mode*>(_v) = _t->mode(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->origin(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->minScaleArc(); break;
        case 5: *reinterpret_cast<double*>(_v) = _t->maxScaleArc(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setLineWidth(*reinterpret_cast<int*>(_v)); break;
        case 1: _t->setFrameShadow(*reinterpret_cast<enum Shadow*>(_v)); break;
        case 2: _t->setMode(*reinterpret_cast<enum Mode*>(_v)); break;
        case 3: _t->setOrigin(*reinterpret_cast<double*>(_v)); break;
        case 4: _t->setMinScaleArc(*reinterpret_cast<double*>(_v)); break;
        case 5: _t->setMaxScaleArc(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtDial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtDial::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QwtDialE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractSlider::qt_metacast(_clname);
}

int QwtDial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
