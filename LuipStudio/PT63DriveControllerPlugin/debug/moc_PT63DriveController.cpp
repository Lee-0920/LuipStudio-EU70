/****************************************************************************
** Meta object code from reading C++ file 'PT63DriveController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../PT63DriveController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PT63DriveController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__PT63DriveController_t {
    QByteArrayData data[5];
    char stringdata0[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__PT63DriveController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__PT63DriveController_t qt_meta_stringdata_Controller__PT63DriveController = {
    {
QT_MOC_LITERAL(0, 0, 31), // "Controller::PT63DriveController"
QT_MOC_LITERAL(1, 32, 19), // "BoxFanControlSignal"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 4), // "temp"
QT_MOC_LITERAL(4, 58, 17) // "BoxFanControlSlot"

    },
    "Controller::PT63DriveController\0"
    "BoxFanControlSignal\0\0temp\0BoxFanControlSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__PT63DriveController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Float,    3,

       0        // eod
};

void Controller::PT63DriveController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PT63DriveController *_t = static_cast<PT63DriveController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->BoxFanControlSignal((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->BoxFanControlSlot((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PT63DriveController::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PT63DriveController::BoxFanControlSignal)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Controller::PT63DriveController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Controller__PT63DriveController.data,
      qt_meta_data_Controller__PT63DriveController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::PT63DriveController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::PT63DriveController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__PT63DriveController.stringdata0))
        return static_cast<void*>(const_cast< PT63DriveController*>(this));
    if (!strcmp(_clname, "BaseController"))
        return static_cast< BaseController*>(const_cast< PT63DriveController*>(this));
    return QObject::qt_metacast(_clname);
}

int Controller::PT63DriveController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Controller::PT63DriveController::BoxFanControlSignal(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
