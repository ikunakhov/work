#include <Video.h>
Video_inf_err get_video_info_qt(QString input_name)
{
    Video_inf_err ret; ret.err = 0;
    int err;
    AVCodecContext		*codec_context_pl = NULL;
    AVFormatContext	   *format_context_pl = NULL;
    AVCodec						*codec_pl = NULL;
    int video_stream_pl = 0;
    av_register_all();
    QByteArray ba;
    ba = input_name.toLatin1();
    const char* filename; filename = ba.data();

    err = avformat_open_input(&format_context_pl, filename, NULL, NULL);
    if (err < 0) {
        ret.err = -1;
        ret.txt_err = QString(" Unable to open input file ");
        return ret;
    }
    err = avformat_find_stream_info(format_context_pl, NULL);
    if (err < 0) {
        ret.err = -1;
        ret.txt_err = QString(" Unable to find stream info  ");
        return ret;
    }

    for (video_stream_pl = 0; video_stream_pl < format_context_pl->nb_streams; ++video_stream_pl) {
        if (format_context_pl->streams[video_stream_pl]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }
    if (video_stream_pl == format_context_pl->nb_streams) {
        ret.err = -1;
        ret.txt_err = QString(" Unable to find video stream  ");
        return ret;
    }

    codec_context_pl = format_context_pl->streams[video_stream_pl]->codec;
    codec_pl = avcodec_find_decoder(codec_context_pl->codec_id);
    err = avcodec_open2(codec_context_pl, codec_pl, NULL);
    if (err < 0) {
        ret.err = -1;
        ret.txt_err = QString(" Unable to open codec  ");
        return ret;
    }
    ret.info.width = QString::number(codec_context_pl->width);
    ret.info.height = QString::number(codec_context_pl->height);
    ret.info.bitrate = QString::number(format_context_pl->bit_rate / 1000);

    const char* cdec = avcodec_get_name(codec_pl->id);
    QString cdec_name_v(cdec);
    ret.info.codec = cdec_name_v;
    int real_sec = format_context_pl->duration / 1000000;
    int h = real_sec / 3600;
    int m = real_sec / 60 - h * 60;
    int s = real_sec - m * 60 - h * 3600;
    QTime dur(h, m, s);
    QString duration = dur.toString();

    ret.info.duration = duration;
    return ret;
}