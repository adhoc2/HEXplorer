/****************************************************************************
** Meta object code from reading C++ file 'qwt_slider.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_slider.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_slider.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9QwtSliderE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtSlider::qt_create_metaobjectdata<qt_meta_tag_ZN9QwtSliderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtSlider",
        "orientation",
        "Qt::Orientation",
        "scalePosition",
        "ScalePosition",
        "trough",
        "groove",
        "handleSize",
        "QSize",
        "borderWidth",
        "spacing",
        "NoScale",
        "LeadingScale",
        "TrailingScale"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
        // property 'orientation'
        QtMocHelpers::PropertyData<Qt::Orientation>(1, 0x80000000 | 2, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'scalePosition'
        QtMocHelpers::PropertyData<enum ScalePosition>(3, 0x80000000 | 4, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'trough'
        QtMocHelpers::PropertyData<bool>(5, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'groove'
        QtMocHelpers::PropertyData<bool>(6, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'handleSize'
        QtMocHelpers::PropertyData<QSize>(7, 0x80000000 | 8, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'borderWidth'
        QtMocHelpers::PropertyData<int>(9, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
        // property 'spacing'
        QtMocHelpers::PropertyData<int>(10, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'ScalePosition'
        QtMocHelpers::EnumData<enum ScalePosition>(4, 4, QMC::EnumFlags{}).add({
            {   11, ScalePosition::NoScale },
            {   12, ScalePosition::LeadingScale },
            {   13, ScalePosition::TrailingScale },
        }),
    };
    return QtMocHelpers::metaObjectData<QwtSlider, qt_meta_tag_ZN9QwtSliderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtAbstractSlider::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtSliderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtSliderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9QwtSliderE_t>.metaTypes,
    nullptr
} };

void QwtSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtSlider *>(_o);
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<Qt::Orientation*>(_v) = _t->orientation(); break;
        case 1: *reinterpret_cast<enum ScalePosition*>(_v) = _t->scalePosition(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->hasTrough(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->hasGroove(); break;
        case 4: *reinterpret_cast<QSize*>(_v) = _t->handleSize(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->borderWidth(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->spacing(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setOrientation(*reinterpret_cast<Qt::Orientation*>(_v)); break;
        case 1: _t->setScalePosition(*reinterpret_cast<enum ScalePosition*>(_v)); break;
        case 2: _t->setTrough(*reinterpret_cast<bool*>(_v)); break;
        case 3: _t->setGroove(*reinterpret_cast<bool*>(_v)); break;
        case 4: _t->setHandleSize(*reinterpret_cast<QSize*>(_v)); break;
        case 5: _t->setBorderWidth(*reinterpret_cast<int*>(_v)); break;
        case 6: _t->setSpacing(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtSliderE_t>.strings))
        return static_cast<void*>(this);
    return QwtAbstractSlider::qt_metacast(_clname);
}

int QwtSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtAbstractSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
