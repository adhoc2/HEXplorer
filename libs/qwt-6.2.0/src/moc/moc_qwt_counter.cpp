/****************************************************************************
** Meta object code from reading C++ file 'qwt_counter.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_counter.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_counter.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10QwtCounterE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtCounter::qt_create_metaobjectdata<qt_meta_tag_ZN10QwtCounterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtCounter",
        "buttonReleased",
        "",
        "value",
        "valueChanged",
        "setValue",
        "btnReleased",
        "btnClicked",
        "textChanged",
        "minimum",
        "maximum",
        "singleStep",
        "numButtons",
        "stepButton1",
        "stepButton2",
        "stepButton3",
        "readOnly",
        "wrapping"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'buttonReleased'
        QtMocHelpers::SignalData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Signal 'valueChanged'
        QtMocHelpers::SignalData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Slot 'setValue'
        QtMocHelpers::SlotData<void(double)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 },
        }}),
        // Slot 'btnReleased'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'btnClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'textChanged'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'value'
        QtMocHelpers::PropertyData<double>(3, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet | QMC::User, 1),
        // property 'minimum'
        QtMocHelpers::PropertyData<double>(9, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'maximum'
        QtMocHelpers::PropertyData<double>(10, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'singleStep'
        QtMocHelpers::PropertyData<double>(11, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'numButtons'
        QtMocHelpers::PropertyData<int>(12, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'stepButton1'
        QtMocHelpers::PropertyData<int>(13, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'stepButton2'
        QtMocHelpers::PropertyData<int>(14, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'stepButton3'
        QtMocHelpers::PropertyData<int>(15, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'readOnly'
        QtMocHelpers::PropertyData<bool>(16, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'wrapping'
        QtMocHelpers::PropertyData<bool>(17, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QwtCounter, qt_meta_tag_ZN10QwtCounterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtCounter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10QwtCounterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10QwtCounterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10QwtCounterE_t>.metaTypes,
    nullptr
} };

void QwtCounter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtCounter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->buttonReleased((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->valueChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->setValue((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->btnReleased(); break;
        case 4: _t->btnClicked(); break;
        case 5: _t->textChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtCounter::*)(double )>(_a, &QwtCounter::buttonReleased, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtCounter::*)(double )>(_a, &QwtCounter::valueChanged, 1))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<double*>(_v) = _t->value(); break;
        case 1: *reinterpret_cast<double*>(_v) = _t->minimum(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->maximum(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->singleStep(); break;
        case 4: *reinterpret_cast<int*>(_v) = _t->numButtons(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->stepButton1(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->stepButton2(); break;
        case 7: *reinterpret_cast<int*>(_v) = _t->stepButton3(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->isReadOnly(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->wrapping(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast<double*>(_v)); break;
        case 1: _t->setMinimum(*reinterpret_cast<double*>(_v)); break;
        case 2: _t->setMaximum(*reinterpret_cast<double*>(_v)); break;
        case 3: _t->setSingleStep(*reinterpret_cast<double*>(_v)); break;
        case 4: _t->setNumButtons(*reinterpret_cast<int*>(_v)); break;
        case 5: _t->setStepButton1(*reinterpret_cast<int*>(_v)); break;
        case 6: _t->setStepButton2(*reinterpret_cast<int*>(_v)); break;
        case 7: _t->setStepButton3(*reinterpret_cast<int*>(_v)); break;
        case 8: _t->setReadOnly(*reinterpret_cast<bool*>(_v)); break;
        case 9: _t->setWrapping(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtCounter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtCounter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10QwtCounterE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QwtCounter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QwtCounter::buttonReleased(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QwtCounter::valueChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
