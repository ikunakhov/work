
#ifndef _AUDIO_H
#define _AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QString>
#include <QTime>



extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include "SDL.h"
}

#define MAX_AUDIO_FRAME_SIZE 192000
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

struct Frame_size {
    int width;
    int height;
};

struct Audio_info_Qt {
    QString format;
    QString frequency;
    QString codec;
    QString duration;
    QString chanels;
    QString bitrate;
};
struct Transcoding_Audio {
    AVCodecID      codec_id;
    uint64_t       chanel_layout;
    AVSampleFormat sample_fmt;
    int64_t        bit_rate;
    int            sample_rate;
};
struct Transcoding_Video {
    AVCodecID      codec_id;
    Frame_size     f_size;
    uint64_t       bit_rate;
};

struct Transcoding_context {
    Transcoding_Audio audio_context;
    Transcoding_Video video_context;
};


static AVFormatContext *ifmt_ctx;
static AVFormatContext *ofmt_ctx;


typedef struct FilteringContext {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
} FilteringContext;


static FilteringContext *filter_ctx;


struct Audio_inf_err {
    Audio_info_Qt info;
    int err;
    QString txt_err;
};
struct Audio_c_err {
    int err;
    QString txt_err;
};
static Audio_c_err open_input_file(const char *filename);
static Audio_c_err open_output_file(const char *filename, Transcoding_context init_context);
static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, const char *filter_spec);
static Audio_c_err init_filters(void);
static int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);
static int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
static int flush_encoder(unsigned int stream_index);

Audio_inf_err get_audio_info_qt(QString input_name);
AVCodecID qstring_to_sup_codec_id(QString codec_name);


struct ret_werr {
        int code_err;
        QString output;
};

struct ret_audio_info {
        ret_werr err;
        QString format;
        QString frequency;
        int chanels;
        int bitrate;
        QString codec;
        QTime duration;
};


    QTime int_to_time(int time_in);


    
#endif