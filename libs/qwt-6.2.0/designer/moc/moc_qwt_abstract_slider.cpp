/****************************************************************************
** Meta object code from reading C++ file 'qwt_abstract_slider.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_abstract_slider.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_abstract_slider.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17QwtAbstractSliderE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtAbstractSlider::qt_create_metaobjectdata<qt_meta_tag_ZN17QwtAbstractSliderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtAbstractSlider",
        "valueChanged",
        "",
        "value",
        "sliderPressed",
        "sliderReleased",
        "sliderMoved",
        "setValue",
        "totalSteps",
        "singleSteps",
        "pageSteps",
        "stepAlignment",
        "readOnly",
        "tracking",
        "wrapping",
        "invertedControls"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'valueChanged'
        QtMocHelpers::SignalData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Signal 'sliderPressed'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sliderReleased'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sliderMoved'
        QtMocHelpers::SignalData<void(double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Slot 'setValue'
        QtMocHelpers::SlotData<void(double)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'value'
        QtMocHelpers::PropertyData<double>(3, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet | QMC::User, 0),
        // property 'totalSteps'
        QtMocHelpers::PropertyData<uint>(8, QMetaType::UInt, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'singleSteps'
        QtMocHelpers::PropertyData<uint>(9, QMetaType::UInt, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'pageSteps'
        QtMocHelpers::PropertyData<uint>(10, QMetaType::UInt, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'stepAlignment'
        QtMocHelpers::PropertyData<bool>(11, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'readOnly'
        QtMocHelpers::PropertyData<bool>(12, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'tracking'
        QtMocHelpers::PropertyData<bool>(13, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'wrapping'
        QtMocHelpers::PropertyData<bool>(14, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'invertedControls'
        QtMocHelpers::PropertyData<bool>(15, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtAbstractSlider, qt_meta_tag_ZN17QwtAbstractSliderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtAbstractSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractScale::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17QwtAbstractSliderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17QwtAbstractSliderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17QwtAbstractSliderE_t>.metaTypes,
    nullptr
} };

void QwtAbstractSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtAbstractSlider *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->sliderPressed(); break;
        case 2: _t->sliderReleased(); break;
        case 3: _t->sliderMoved((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->setValue((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtAbstractSlider::*)(double )>(_a, &QwtAbstractSlider::valueChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtAbstractSlider::*)()>(_a, &QwtAbstractSlider::sliderPressed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtAbstractSlider::*)()>(_a, &QwtAbstractSlider::sliderReleased, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtAbstractSlider::*)(double )>(_a, &QwtAbstractSlider::sliderMoved, 3))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<double*>(_v) = _t->value(); break;
        case 1: *reinterpret_cast<uint*>(_v) = _t->totalSteps(); break;
        case 2: *reinterpret_cast<uint*>(_v) = _t->singleSteps(); break;
        case 3: *reinterpret_cast<uint*>(_v) = _t->pageSteps(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->stepAlignment(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->isReadOnly(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->isTracking(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->wrapping(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->invertedControls(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast<double*>(_v)); break;
        case 1: _t->setTotalSteps(*reinterpret_cast<uint*>(_v)); break;
        case 2: _t->setSingleSteps(*reinterpret_cast<uint*>(_v)); break;
        case 3: _t->setPageSteps(*reinterpret_cast<uint*>(_v)); break;
        case 4: _t->setStepAlignment(*reinterpret_cast<bool*>(_v)); break;
        case 5: _t->setReadOnly(*reinterpret_cast<bool*>(_v)); break;
        case 6: _t->setTracking(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setWrapping(*reinterpret_cast<bool*>(_v)); break;
        case 8: _t->setInvertedControls(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtAbstractSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtAbstractSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17QwtAbstractSliderE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractScale::qt_metacast(_clname);
}

int QwtAbstractSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractScale::qt_metacall(_c, _id, _a);
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
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QwtAbstractSlider::valueChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QwtAbstractSlider::sliderPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QwtAbstractSlider::sliderReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QwtAbstractSlider::sliderMoved(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
