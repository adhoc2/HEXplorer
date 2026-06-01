/****************************************************************************
** Meta object code from reading C++ file 'qwt_wheel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_wheel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_wheel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8QwtWheelE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtWheel::qt_create_metaobjectdata<qt_meta_tag_ZN8QwtWheelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtWheel",
        "valueChanged",
        "",
        "value",
        "wheelPressed",
        "wheelReleased",
        "wheelMoved",
        "setValue",
        "setTotalAngle",
        "setViewAngle",
        "setMass",
        "orientation",
        "Qt::Orientation",
        "minimum",
        "maximum",
        "singleStep",
        "pageStepCount",
        "stepAlignment",
        "tracking",
        "wrapping",
        "inverted",
        "mass",
        "updateInterval",
        "totalAngle",
        "viewAngle",
        "tickCount",
        "wheelWidth",
        "borderWidth",
        "wheelBorderWidth"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'valueChanged'
        QtMocHelpers::SignalData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Signal 'wheelPressed'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wheelReleased'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wheelMoved'
        QtMocHelpers::SignalData<void(double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Slot 'setValue'
        QtMocHelpers::SlotData<void(double)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
        // Slot 'setTotalAngle'
        QtMocHelpers::SlotData<void(double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
        // Slot 'setViewAngle'
        QtMocHelpers::SlotData<void(double)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
        // Slot 'setMass'
        QtMocHelpers::SlotData<void(double)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'orientation'
        QtMocHelpers::PropertyData<Qt::Orientation>(11, 0x80000000 | 12, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'value'
        QtMocHelpers::PropertyData<double>(3, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet | QMC::User, 0),
        // property 'minimum'
        QtMocHelpers::PropertyData<double>(13, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'maximum'
        QtMocHelpers::PropertyData<double>(14, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'singleStep'
        QtMocHelpers::PropertyData<double>(15, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'pageStepCount'
        QtMocHelpers::PropertyData<int>(16, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'stepAlignment'
        QtMocHelpers::PropertyData<bool>(17, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tracking'
        QtMocHelpers::PropertyData<bool>(18, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'wrapping'
        QtMocHelpers::PropertyData<bool>(19, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'inverted'
        QtMocHelpers::PropertyData<bool>(20, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'mass'
        QtMocHelpers::PropertyData<double>(21, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'updateInterval'
        QtMocHelpers::PropertyData<int>(22, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'totalAngle'
        QtMocHelpers::PropertyData<double>(23, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'viewAngle'
        QtMocHelpers::PropertyData<double>(24, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tickCount'
        QtMocHelpers::PropertyData<int>(25, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'wheelWidth'
        QtMocHelpers::PropertyData<int>(26, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'borderWidth'
        QtMocHelpers::PropertyData<int>(27, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'wheelBorderWidth'
        QtMocHelpers::PropertyData<int>(28, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtWheel, qt_meta_tag_ZN8QwtWheelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtWheel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QwtWheelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QwtWheelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8QwtWheelE_t>.metaTypes,
    nullptr
} };

void QwtWheel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtWheel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->wheelPressed(); break;
        case 2: _t->wheelReleased(); break;
        case 3: _t->wheelMoved((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->setValue((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 5: _t->setTotalAngle((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 6: _t->setViewAngle((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 7: _t->setMass((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtWheel::*)(double )>(_a, &QwtWheel::valueChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtWheel::*)()>(_a, &QwtWheel::wheelPressed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtWheel::*)()>(_a, &QwtWheel::wheelReleased, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtWheel::*)(double )>(_a, &QwtWheel::wheelMoved, 3))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<Qt::Orientation*>(_v) = _t->orientation(); break;
        case 1: *reinterpret_cast<double*>(_v) = _t->value(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->minimum(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->maximum(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->singleStep(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->pageStepCount(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->stepAlignment(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->isTracking(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->wrapping(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->isInverted(); break;
        case 10: *reinterpret_cast<double*>(_v) = _t->mass(); break;
        case 11: *reinterpret_cast<int*>(_v) = _t->updateInterval(); break;
        case 12: *reinterpret_cast<double*>(_v) = _t->totalAngle(); break;
        case 13: *reinterpret_cast<double*>(_v) = _t->viewAngle(); break;
        case 14: *reinterpret_cast<int*>(_v) = _t->tickCount(); break;
        case 15: *reinterpret_cast<int*>(_v) = _t->wheelWidth(); break;
        case 16: *reinterpret_cast<int*>(_v) = _t->borderWidth(); break;
        case 17: *reinterpret_cast<int*>(_v) = _t->wheelBorderWidth(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setOrientation(*reinterpret_cast<Qt::Orientation*>(_v)); break;
        case 1: _t->setValue(*reinterpret_cast<double*>(_v)); break;
        case 2: _t->setMinimum(*reinterpret_cast<double*>(_v)); break;
        case 3: _t->setMaximum(*reinterpret_cast<double*>(_v)); break;
        case 4: _t->setSingleStep(*reinterpret_cast<double*>(_v)); break;
        case 5: _t->setPageStepCount(*reinterpret_cast<int*>(_v)); break;
        case 6: _t->setStepAlignment(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setTracking(*reinterpret_cast<bool*>(_v)); break;
        case 8: _t->setWrapping(*reinterpret_cast<bool*>(_v)); break;
        case 9: _t->setInverted(*reinterpret_cast<bool*>(_v)); break;
        case 10: _t->setMass(*reinterpret_cast<double*>(_v)); break;
        case 11: _t->setUpdateInterval(*reinterpret_cast<int*>(_v)); break;
        case 12: _t->setTotalAngle(*reinterpret_cast<double*>(_v)); break;
        case 13: _t->setViewAngle(*reinterpret_cast<double*>(_v)); break;
        case 14: _t->setTickCount(*reinterpret_cast<int*>(_v)); break;
        case 15: _t->setWheelWidth(*reinterpret_cast<int*>(_v)); break;
        case 16: _t->setBorderWidth(*reinterpret_cast<int*>(_v)); break;
        case 17: _t->setWheelBorderWidth(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtWheel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtWheel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QwtWheelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QwtWheel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void QwtWheel::valueChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QwtWheel::wheelPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QwtWheel::wheelReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QwtWheel::wheelMoved(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
