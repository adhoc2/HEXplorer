/****************************************************************************
** Meta object code from reading C++ file 'qwt_abstract_scale.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_abstract_scale.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_abstract_scale.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16QwtAbstractScaleE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtAbstractScale::qt_create_metaobjectdata<qt_meta_tag_ZN16QwtAbstractScaleE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtAbstractScale",
        "lowerBound",
        "upperBound",
        "scaleMaxMajor",
        "scaleMaxMinor",
        "scaleStepSize"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
        // property 'lowerBound'
        QtMocHelpers::PropertyData<double>(1, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'upperBound'
        QtMocHelpers::PropertyData<double>(2, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'scaleMaxMajor'
        QtMocHelpers::PropertyData<int>(3, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'scaleMaxMinor'
        QtMocHelpers::PropertyData<int>(4, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'scaleStepSize'
        QtMocHelpers::PropertyData<double>(5, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtAbstractScale, qt_meta_tag_ZN16QwtAbstractScaleE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtAbstractScale::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16QwtAbstractScaleE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16QwtAbstractScaleE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16QwtAbstractScaleE_t>.metaTypes,
    nullptr
} };

void QwtAbstractScale::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtAbstractScale *>(_o);
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<double*>(_v) = _t->lowerBound(); break;
        case 1: *reinterpret_cast<double*>(_v) = _t->upperBound(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->scaleMaxMajor(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->scaleMaxMinor(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->scaleStepSize(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setLowerBound(*reinterpret_cast<double*>(_v)); break;
        case 1: _t->setUpperBound(*reinterpret_cast<double*>(_v)); break;
        case 2: _t->setScaleMaxMajor(*reinterpret_cast<int*>(_v)); break;
        case 3: _t->setScaleMaxMinor(*reinterpret_cast<int*>(_v)); break;
        case 4: _t->setScaleStepSize(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtAbstractScale::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtAbstractScale::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16QwtAbstractScaleE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QwtAbstractScale::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
