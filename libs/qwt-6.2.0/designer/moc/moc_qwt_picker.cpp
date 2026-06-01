/****************************************************************************
** Meta object code from reading C++ file 'qwt_picker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qwt_picker.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_picker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9QwtPickerE_t {};
} // unnamed namespace

template <> constexpr inline auto QwtPicker::qt_create_metaobjectdata<qt_meta_tag_ZN9QwtPickerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QwtPicker",
        "activated",
        "",
        "on",
        "selected",
        "QPolygon",
        "polygon",
        "appended",
        "QPoint",
        "pos",
        "moved",
        "removed",
        "changed",
        "selection",
        "setEnabled",
        "isEnabled",
        "resizeMode",
        "ResizeMode",
        "trackerMode",
        "DisplayMode",
        "trackerPen",
        "QPen",
        "trackerFont",
        "QFont",
        "rubberBand",
        "RubberBand",
        "rubberBandPen",
        "NoRubberBand",
        "HLineRubberBand",
        "VLineRubberBand",
        "CrossRubberBand",
        "RectRubberBand",
        "EllipseRubberBand",
        "PolygonRubberBand",
        "UserRubberBand",
        "AlwaysOff",
        "AlwaysOn",
        "ActiveOnly",
        "Stretch",
        "KeepSize"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'activated'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'selected'
        QtMocHelpers::SignalData<void(const QPolygon &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'appended'
        QtMocHelpers::SignalData<void(const QPoint &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'moved'
        QtMocHelpers::SignalData<void(const QPoint &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'removed'
        QtMocHelpers::SignalData<void(const QPoint &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'changed'
        QtMocHelpers::SignalData<void(const QPolygon &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 13 },
        }}),
        // Slot 'setEnabled'
        QtMocHelpers::SlotData<void(bool)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isEnabled'
        QtMocHelpers::PropertyData<bool>(15, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable),
        // property 'resizeMode'
        QtMocHelpers::PropertyData<enum ResizeMode>(16, 0x80000000 | 17, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'trackerMode'
        QtMocHelpers::PropertyData<enum DisplayMode>(18, 0x80000000 | 19, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'trackerPen'
        QtMocHelpers::PropertyData<QPen>(20, 0x80000000 | 21, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'trackerFont'
        QtMocHelpers::PropertyData<QFont>(22, 0x80000000 | 23, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'rubberBand'
        QtMocHelpers::PropertyData<enum RubberBand>(24, 0x80000000 | 25, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
        // property 'rubberBandPen'
        QtMocHelpers::PropertyData<QPen>(26, 0x80000000 | 21, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'RubberBand'
        QtMocHelpers::EnumData<enum RubberBand>(25, 25, QMC::EnumFlags{}).add({
            {   27, RubberBand::NoRubberBand },
            {   28, RubberBand::HLineRubberBand },
            {   29, RubberBand::VLineRubberBand },
            {   30, RubberBand::CrossRubberBand },
            {   31, RubberBand::RectRubberBand },
            {   32, RubberBand::EllipseRubberBand },
            {   33, RubberBand::PolygonRubberBand },
            {   34, RubberBand::UserRubberBand },
        }),
        // enum 'DisplayMode'
        QtMocHelpers::EnumData<enum DisplayMode>(19, 19, QMC::EnumFlags{}).add({
            {   35, DisplayMode::AlwaysOff },
            {   36, DisplayMode::AlwaysOn },
            {   37, DisplayMode::ActiveOnly },
        }),
        // enum 'ResizeMode'
        QtMocHelpers::EnumData<enum ResizeMode>(17, 17, QMC::EnumFlags{}).add({
            {   38, ResizeMode::Stretch },
            {   39, ResizeMode::KeepSize },
        }),
    };
    return QtMocHelpers::metaObjectData<QwtPicker, qt_meta_tag_ZN9QwtPickerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QwtPicker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtPickerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtPickerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9QwtPickerE_t>.metaTypes,
    nullptr
} };

void QwtPicker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QwtPicker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->activated((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->selected((*reinterpret_cast<std::add_pointer_t<QPolygon>>(_a[1]))); break;
        case 2: _t->appended((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 3: _t->moved((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 4: _t->removed((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 5: _t->changed((*reinterpret_cast<std::add_pointer_t<QPolygon>>(_a[1]))); break;
        case 6: _t->setEnabled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QwtPicker::*)(bool )>(_a, &QwtPicker::activated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPicker::*)(const QPolygon & )>(_a, &QwtPicker::selected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPicker::*)(const QPoint & )>(_a, &QwtPicker::appended, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPicker::*)(const QPoint & )>(_a, &QwtPicker::moved, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPicker::*)(const QPoint & )>(_a, &QwtPicker::removed, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (QwtPicker::*)(const QPolygon & )>(_a, &QwtPicker::changed, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isEnabled(); break;
        case 1: *reinterpret_cast<enum ResizeMode*>(_v) = _t->resizeMode(); break;
        case 2: *reinterpret_cast<enum DisplayMode*>(_v) = _t->trackerMode(); break;
        case 3: *reinterpret_cast<QPen*>(_v) = _t->trackerPen(); break;
        case 4: *reinterpret_cast<QFont*>(_v) = _t->trackerFont(); break;
        case 5: *reinterpret_cast<enum RubberBand*>(_v) = _t->rubberBand(); break;
        case 6: *reinterpret_cast<QPen*>(_v) = _t->rubberBandPen(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 1: _t->setResizeMode(*reinterpret_cast<enum ResizeMode*>(_v)); break;
        case 2: _t->setTrackerMode(*reinterpret_cast<enum DisplayMode*>(_v)); break;
        case 3: _t->setTrackerPen(*reinterpret_cast<QPen*>(_v)); break;
        case 4: _t->setTrackerFont(*reinterpret_cast<QFont*>(_v)); break;
        case 5: _t->setRubberBand(*reinterpret_cast<enum RubberBand*>(_v)); break;
        case 6: _t->setRubberBandPen(*reinterpret_cast<QPen*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *QwtPicker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtPicker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QwtPickerE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QwtEventPattern"))
        return static_cast< QwtEventPattern*>(this);
    return QObject::qt_metacast(_clname);
}

int QwtPicker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QwtPicker::activated(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QwtPicker::selected(const QPolygon & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void QwtPicker::appended(const QPoint & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void QwtPicker::moved(const QPoint & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void QwtPicker::removed(const QPoint & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void QwtPicker::changed(const QPolygon & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
