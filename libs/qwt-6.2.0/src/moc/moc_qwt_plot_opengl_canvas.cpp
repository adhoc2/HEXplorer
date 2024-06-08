/****************************************************************************
** Meta object code from reading C++ file 'qwt_plot_opengl_canvas.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_plot_opengl_canvas.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_plot_opengl_canvas.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.0. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSQwtPlotOpenGLCanvasENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSQwtPlotOpenGLCanvasENDCLASS = QtMocHelpers::stringData(
    "QwtPlotOpenGLCanvas",
    "replot",
    "",
    "invalidateBackingStore",
    "borderPath",
    "QPainterPath",
    "frameShadow",
    "QFrame::Shadow",
    "frameShape",
    "QFrame::Shape",
    "lineWidth",
    "midLineWidth",
    "frameWidth",
    "frameRect",
    "borderRadius"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQwtPlotOpenGLCanvasENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       7,   37, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x0a,    8 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   33,    2, 0x02,    9 /* Public */,
       4,    1,   34,    2, 0x102,   10 /* Public | MethodIsConst  */,

 // slots: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    0x80000000 | 5, QMetaType::QRect,    2,

 // properties: name, type, flags
       6, 0x80000000 | 7, 0x0001510b, uint(-1), 0,
       8, 0x80000000 | 9, 0x0001510b, uint(-1), 0,
      10, QMetaType::Int, 0x00015103, uint(-1), 0,
      11, QMetaType::Int, 0x00015103, uint(-1), 0,
      12, QMetaType::Int, 0x00015001, uint(-1), 0,
      13, QMetaType::QRect, 0x00014001, uint(-1), 0,
      14, QMetaType::Double, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT static const QMetaObject::SuperData qt_meta_extradata_CLASSQwtPlotOpenGLCanvasENDCLASS[] = {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    nullptr
};

Q_CONSTINIT const QMetaObject QwtPlotOpenGLCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSQwtPlotOpenGLCanvasENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQwtPlotOpenGLCanvasENDCLASS,
    qt_static_metacall,
    qt_meta_extradata_CLASSQwtPlotOpenGLCanvasENDCLASS,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQwtPlotOpenGLCanvasENDCLASS_t,
        // property 'frameShadow'
        QtPrivate::TypeAndForceComplete<QFrame::Shadow, std::true_type>,
        // property 'frameShape'
        QtPrivate::TypeAndForceComplete<QFrame::Shape, std::true_type>,
        // property 'lineWidth'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'midLineWidth'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'frameWidth'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'frameRect'
        QtPrivate::TypeAndForceComplete<QRect, std::true_type>,
        // property 'borderRadius'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QwtPlotOpenGLCanvas, std::true_type>,
        // method 'replot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'invalidateBackingStore'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'borderPath'
        QtPrivate::TypeAndForceComplete<QPainterPath, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QRect &, std::false_type>
    >,
    nullptr
} };

void QwtPlotOpenGLCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QwtPlotOpenGLCanvas *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->replot(); break;
        case 1: _t->invalidateBackingStore(); break;
        case 2: { QPainterPath _r = _t->borderPath((*reinterpret_cast< std::add_pointer_t<QRect>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QPainterPath*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<QwtPlotOpenGLCanvas *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QFrame::Shadow*>(_v) = _t->frameShadow(); break;
        case 1: *reinterpret_cast< QFrame::Shape*>(_v) = _t->frameShape(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->lineWidth(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->midLineWidth(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->frameWidth(); break;
        case 5: *reinterpret_cast< QRect*>(_v) = _t->frameRect(); break;
        case 6: *reinterpret_cast< double*>(_v) = _t->borderRadius(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<QwtPlotOpenGLCanvas *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFrameShadow(*reinterpret_cast< QFrame::Shadow*>(_v)); break;
        case 1: _t->setFrameShape(*reinterpret_cast< QFrame::Shape*>(_v)); break;
        case 2: _t->setLineWidth(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setMidLineWidth(*reinterpret_cast< int*>(_v)); break;
        case 6: _t->setBorderRadius(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *QwtPlotOpenGLCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QwtPlotOpenGLCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQwtPlotOpenGLCanvasENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QwtPlotAbstractGLCanvas"))
        return static_cast< QwtPlotAbstractGLCanvas*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int QwtPlotOpenGLCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
