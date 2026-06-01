/****************************************************************************
** Meta object code from reading C++ file 'qwt_thermo.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_thermo.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_thermo.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9QwtThermoE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtThermo::qt_create_metaobjectdata<qt_meta_tag_ZN9QwtThermoE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtThermo",
        "setValue",
        "",
        "orientation",
        "Qt::Orientation",
        "scalePosition",
        "ScalePosition",
        "originMode",
        "OriginMode",
        "alarmEnabled",
        "alarmLevel",
        "origin",
        "spacing",
        "borderWidth",
        "pipeWidth",
        "value",
        "NoScale",
        "LeadingScale",
        "TrailingScale",
        "OriginMinimum",
        "OriginMaximum",
        "OriginCustom"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'setValue'
        QtMocHelpers::SlotData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'orientation'
        QtMocHelpers::PropertyData<Qt::Orientation>(3, 0x80000000 | 4, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'scalePosition'
        QtMocHelpers::PropertyData<enum ScalePosition>(5, 0x80000000 | 6, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'originMode'
        QtMocHelpers::PropertyData<enum OriginMode>(7, 0x80000000 | 8, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'alarmEnabled'
        QtMocHelpers::PropertyData<bool>(9, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'alarmLevel'
        QtMocHelpers::PropertyData<double>(10, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'origin'
        QtMocHelpers::PropertyData<double>(11, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'spacing'
        QtMocHelpers::PropertyData<int>(12, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'borderWidth'
        QtMocHelpers::PropertyData<int>(13, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'pipeWidth'
        QtMocHelpers::PropertyData<int>(14, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'value'
        QtMocHelpers::PropertyData<double>(15, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet | QMC::User),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'ScalePosition'
        QtMocHelpers::EnumData<enum ScalePosition>(6, 6, QMC::EnumFlags{}).add({
            {   16, ScalePosition::NoScale },
            {   17, ScalePosition::LeadingScale },
            {   18, ScalePosition::TrailingScale },
        }),
        // enum 'OriginMode'
        QtMocHelpers::EnumData<enum OriginMode>(8, 8, QMC::EnumFlags{}).add({
            {   19, OriginMode::OriginMinimum },
            {   20, OriginMode::OriginMaximum },
            {   21, OriginMode::OriginCustom },
        }),
    };
    return QtMocHelpers::metaObjectData<QwtThermo, qt_meta_tag_ZN9QwtThermoE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtThermo::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractScale::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtThermoE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtThermoE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9QwtThermoE_t>.metaTypes,
    nullptr
} };

void QwtThermo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtThermo *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->setValue((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<Qt::Orientation*>(_v) = _t->orientation(); break;
        case 1: *reinterpret_cast<enum ScalePosition*>(_v) = _t->scalePosition(); break;
        case 2: *reinterpret_cast<enum OriginMode*>(_v) = _t->originMode(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->alarmEnabled(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->alarmLevel(); break;
        case 5: *reinterpret_cast<double*>(_v) = _t->origin(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->spacing(); break;
        case 7: *reinterpret_cast<int*>(_v) = _t->borderWidth(); break;
        case 8: *reinterpret_cast<int*>(_v) = _t->pipeWidth(); break;
        case 9: *reinterpret_cast<double*>(_v) = _t->value(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setOrientation(*reinterpret_cast<Qt::Orientation*>(_v)); break;
        case 1: _t->setScalePosition(*reinterpret_cast<enum ScalePosition*>(_v)); break;
        case 2: _t->setOriginMode(*reinterpret_cast<enum OriginMode*>(_v)); break;
        case 3: _t->setAlarmEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 4: _t->setAlarmLevel(*reinterpret_cast<double*>(_v)); break;
        case 5: _t->setOrigin(*reinterpret_cast<double*>(_v)); break;
        case 6: _t->setSpacing(*reinterpret_cast<int*>(_v)); break;
        case 7: _t->setBorderWidth(*reinterpret_cast<int*>(_v)); break;
        case 8: _t->setPipeWidth(*reinterpret_cast<int*>(_v)); break;
        case 9: _t->setValue(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtThermo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtThermo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtThermoE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractScale::qt_metacast(_clname);
}

int QwtThermo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractScale::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
