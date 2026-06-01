/****************************************************************************
** Meta object code from reading C++ file 'qwt_knob.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_knob.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_knob.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN7QwtKnobE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtKnob::qt_create_metaobjectdata<qt_meta_tag_ZN7QwtKnobE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtKnob",
        "knobStyle",
        "KnobStyle",
        "knobWidth",
        "alignment",
        "Qt::Alignment",
        "totalAngle",
        "numTurns",
        "markerStyle",
        "MarkerStyle",
        "markerSize",
        "borderWidth",
        "Flat",
        "Raised",
        "Sunken",
        "Styled",
        "NoMarker",
        "Tick",
        "Triangle",
        "Dot",
        "Nub",
        "Notch"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
        // property 'knobStyle'
        QtMocHelpers::PropertyData<enum KnobStyle>(1, 0x80000000 | 2, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'knobWidth'
        QtMocHelpers::PropertyData<int>(3, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'alignment'
        QtMocHelpers::PropertyData<Qt::Alignment>(4, 0x80000000 | 5, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'totalAngle'
        QtMocHelpers::PropertyData<double>(6, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'numTurns'
        QtMocHelpers::PropertyData<int>(7, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'markerStyle'
        QtMocHelpers::PropertyData<enum MarkerStyle>(8, 0x80000000 | 9, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'markerSize'
        QtMocHelpers::PropertyData<int>(10, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'borderWidth'
        QtMocHelpers::PropertyData<int>(11, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'KnobStyle'
        QtMocHelpers::EnumData<enum KnobStyle>(2, 2, QMC::EnumFlags{}).add({
            {   12, KnobStyle::Flat },
            {   13, KnobStyle::Raised },
            {   14, KnobStyle::Sunken },
            {   15, KnobStyle::Styled },
        }),
        // enum 'MarkerStyle'
        QtMocHelpers::EnumData<enum MarkerStyle>(9, 9, QMC::EnumFlags{}).add({
            {   16, MarkerStyle::NoMarker },
            {   17, MarkerStyle::Tick },
            {   18, MarkerStyle::Triangle },
            {   19, MarkerStyle::Dot },
            {   20, MarkerStyle::Nub },
            {   21, MarkerStyle::Notch },
        }),
    };
    return QtMocHelpers::metaObjectData<QwtKnob, qt_meta_tag_ZN7QwtKnobE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtKnob::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractSlider::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QwtKnobE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QwtKnobE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN7QwtKnobE_t>.metaTypes,
    nullptr
} };

void QwtKnob::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtKnob *>(_o);
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<enum KnobStyle*>(_v) = _t->knobStyle(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->knobWidth(); break;
        case 2: *reinterpret_cast<Qt::Alignment*>(_v) = _t->alignment(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->totalAngle(); break;
        case 4: *reinterpret_cast<int*>(_v) = _t->numTurns(); break;
        case 5: *reinterpret_cast<enum MarkerStyle*>(_v) = _t->markerStyle(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->markerSize(); break;
        case 7: *reinterpret_cast<int*>(_v) = _t->borderWidth(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setKnobStyle(*reinterpret_cast<enum KnobStyle*>(_v)); break;
        case 1: _t->setKnobWidth(*reinterpret_cast<int*>(_v)); break;
        case 2: _t->setAlignment(*reinterpret_cast<Qt::Alignment*>(_v)); break;
        case 3: _t->setTotalAngle(*reinterpret_cast<double*>(_v)); break;
        case 4: _t->setNumTurns(*reinterpret_cast<int*>(_v)); break;
        case 5: _t->setMarkerStyle(*reinterpret_cast<enum MarkerStyle*>(_v)); break;
        case 6: _t->setMarkerSize(*reinterpret_cast<int*>(_v)); break;
        case 7: _t->setBorderWidth(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtKnob::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtKnob::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN7QwtKnobE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractSlider::qt_metacast(_clname);
}

int QwtKnob::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
