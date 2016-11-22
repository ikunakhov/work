#ifndef _VIDEO_H
#define _VIDEO_H
#include <QString>
#include <QTime>
#include <qmessagebox.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


struct Video_info_Qt {
    QString width;
    QString height;
    QString bitrate;
    QString codec;
    QString duration;
};

struct Video_inf_err {
    Video_info_Qt info;
    int err;
    QString txt_err;
};

Video_inf_err get_video_info_qt(QString input_name);

#endif

