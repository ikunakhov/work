/****************************************************************************
** Meta object code from reading C++ file 'one_converter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../one_converter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'one_converter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Ui_ONE_conv_t {
    QByteArrayData data[21];
    char stringdata0[264];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Ui_ONE_conv_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Ui_ONE_conv_t qt_meta_stringdata_Ui_ONE_conv = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Ui_ONE_conv"
QT_MOC_LITERAL(1, 12, 15), // "inf_codec_video"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 16), // "inf_format_video"
QT_MOC_LITERAL(4, 46, 16), // "inf_format_image"
QT_MOC_LITERAL(5, 63, 16), // "inf_format_audio"
QT_MOC_LITERAL(6, 80, 13), // "audio_convert"
QT_MOC_LITERAL(7, 94, 17), // "find_audio_folder"
QT_MOC_LITERAL(8, 112, 10), // "find_audio"
QT_MOC_LITERAL(9, 123, 10), // "find_video"
QT_MOC_LITERAL(10, 134, 16), // "find_player_info"
QT_MOC_LITERAL(11, 151, 16), // "start_wind_video"
QT_MOC_LITERAL(12, 168, 10), // "player_run"
QT_MOC_LITERAL(13, 179, 16), // "start_play_audio"
QT_MOC_LITERAL(14, 196, 11), // "open_dialog"
QT_MOC_LITERAL(15, 208, 13), // "open_dialog_2"
QT_MOC_LITERAL(16, 222, 6), // "cb4_on"
QT_MOC_LITERAL(17, 229, 11), // "pix_size_rb"
QT_MOC_LITERAL(18, 241, 12), // "byte_size_rb"
QT_MOC_LITERAL(19, 254, 4), // "calc"
QT_MOC_LITERAL(20, 259, 4) // "init"

    },
    "Ui_ONE_conv\0inf_codec_video\0\0"
    "inf_format_video\0inf_format_image\0"
    "inf_format_audio\0audio_convert\0"
    "find_audio_folder\0find_audio\0find_video\0"
    "find_player_info\0start_wind_video\0"
    "player_run\0start_play_audio\0open_dialog\0"
    "open_dialog_2\0cb4_on\0pix_size_rb\0"
    "byte_size_rb\0calc\0init"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Ui_ONE_conv[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  109,    2, 0x0a /* Public */,
       3,    1,  112,    2, 0x0a /* Public */,
       4,    1,  115,    2, 0x0a /* Public */,
       5,    1,  118,    2, 0x0a /* Public */,
       6,    0,  121,    2, 0x0a /* Public */,
       7,    0,  122,    2, 0x0a /* Public */,
       8,    0,  123,    2, 0x0a /* Public */,
       9,    0,  124,    2, 0x0a /* Public */,
      10,    0,  125,    2, 0x0a /* Public */,
      11,    0,  126,    2, 0x0a /* Public */,
      12,    0,  127,    2, 0x0a /* Public */,
      13,    0,  128,    2, 0x0a /* Public */,
      14,    0,  129,    2, 0x0a /* Public */,
      15,    0,  130,    2, 0x0a /* Public */,
      16,    0,  131,    2, 0x0a /* Public */,
      17,    0,  132,    2, 0x0a /* Public */,
      18,    0,  133,    2, 0x0a /* Public */,
      19,    0,  134,    2, 0x0a /* Public */,
      20,    0,  135,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Ui_ONE_conv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Ui_ONE_conv *_t = static_cast<Ui_ONE_conv *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->inf_codec_video((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->inf_format_video((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->inf_format_image((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->inf_format_audio((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->audio_convert(); break;
        case 5: _t->find_audio_folder(); break;
        case 6: _t->find_audio(); break;
        case 7: _t->find_video(); break;
        case 8: _t->find_player_info(); break;
        case 9: _t->start_wind_video(); break;
        case 10: _t->player_run(); break;
        case 11: _t->start_play_audio(); break;
        case 12: _t->open_dialog(); break;
        case 13: _t->open_dialog_2(); break;
        case 14: _t->cb4_on(); break;
        case 15: _t->pix_size_rb(); break;
        case 16: _t->byte_size_rb(); break;
        case 17: _t->calc(); break;
        case 18: _t->init(); break;
        default: ;
        }
    }
}

const QMetaObject Ui_ONE_conv::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Ui_ONE_conv.data,
      qt_meta_data_Ui_ONE_conv,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Ui_ONE_conv::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Ui_ONE_conv::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Ui_ONE_conv.stringdata0))
        return static_cast<void*>(const_cast< Ui_ONE_conv*>(this));
    if (!strcmp(_clname, "Ui::ONE_converterClass"))
        return static_cast< Ui::ONE_converterClass*>(const_cast< Ui_ONE_conv*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Ui_ONE_conv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
struct qt_meta_stringdata_Win_t {
    QByteArrayData data[4];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Win_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Win_t qt_meta_stringdata_Win = {
    {
QT_MOC_LITERAL(0, 0, 3), // "Win"
QT_MOC_LITERAL(1, 4, 4), // "calc"
QT_MOC_LITERAL(2, 9, 0), // ""
QT_MOC_LITERAL(3, 10, 4) // "init"

    },
    "Win\0calc\0\0init"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Win[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Win::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Win *_t = static_cast<Win *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->calc(); break;
        case 1: _t->init(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Win::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Win.data,
      qt_meta_data_Win,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Win::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Win::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Win.stringdata0))
        return static_cast<void*>(const_cast< Win*>(this));
    return QWidget::qt_metacast(_clname);
}

int Win::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
