#ifndef _PLAYER_H
#define _PLAYER_H
extern"C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
#include <libavutil/avstring.h>
#include <libavutil/pixfmt.h>
#include <libavutil/log.h>
#include <libavutil/avutil.h>
#include <SDL.h>
#include <SDL_thread.h>
}


#include <stdio.h>
#include <math.h>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000
#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define  MAX_VIDEOQ_SIZE (5 * 256 * 1024)
#define VIDEO_PICTURE_QUEUE_SIZE 1

#define FF_ALLOC_EVENT   (SDL_USEREVENT)
#define FF_REFRESH_EVENT (SDL_USEREVENT + 1)
#define FF_QUIT_EVENT (SDL_USEREVENT + 2)

#define SDL_VIDEO_MODE_BPP 24
#define SDL_VIDEO_MODE_FLAGS SDL_HWSURFACE|SDL_RESIZABLE|SDL_ASYNCBLIT|SDL_HWACCEL
#define VIDEO_PICTURE_QUEUE_SIZE 1
#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0
#define SAMPLE_CORRECTION_PERCENT_MAX 10
#define AUDIO_DIFF_AVG_NB 20

#define DEFAULT_AV_SYNC_TYPE AV_SYNC_VIDEO_MASTER

    enum Preview_type {
        VIDEO_PREVIEW,
        AUDIO_PREVIEW,
        PLAYER_RUN,
    };
    
    enum {
        AV_SYNC_AUDIO_MASTER,
        AV_SYNC_VIDEO_MASTER,
        AV_SYNC_EXTERNAL_MASTER,
    };

    typedef struct PacketQueue {
        AVPacketList *first_pkt, *last_pkt;
        int nb_packets;
        int size;
        SDL_mutex *mutex;
        SDL_cond *cond;
    } PacketQueue;

    typedef struct VideoPicture {
        SDL_Overlay *bmp;
        int width, height;
        int allocated;
        double pts;
    } VideoPicture;




    typedef struct VideoState {
        char            filename[1024];
        AVFormatContext *ic;
        int				seek_req;
        int				seek_flags;
        int				seek_pos;

        VideoPicture	pictq[VIDEO_PICTURE_QUEUE_SIZE];
        int				pictq_size, pictq_rindex, pictq_windex;
        SDL_mutex		*pictq_mutex;
        SDL_cond		*pictq_cond;
        int             videoStream;
        AVStream		*video_st;
        PacketQueue		videoq;
        double			video_clock;

        int64_t			video_current_pts;

        int64_t			video_current_pts_time;
        SDL_Thread		*video_tid;


        int				audioStream;
        AVStream        *audio_st;
        AVFrame         *audio_frame;
        PacketQueue     audioq;
        unsigned int    audio_buf_size;
        unsigned int    audio_buf_index;
        AVPacket        audio_pkt;
        uint8_t         *audio_pkt_data;
        int             audio_pkt_size;
        uint8_t         *audio_buf;
        uint8_t         *audio_buf1;
        DECLARE_ALIGNED(16, uint8_t, audio_buf2)[MAX_AUDIO_FRAME_SIZE * 4];
        enum AVSampleFormat  audio_src_fmt;
        enum AVSampleFormat  audio_tgt_fmt;
        int             audio_src_channels;
        int             audio_tgt_channels;
        int64_t         audio_src_channel_layout;
        int64_t         audio_tgt_channel_layout;
        int             audio_src_freq;
        int             audio_tgt_freq;
        double			audio_clock;
        double			audio_diff_cum;
        double			audio_diff_avg_coef;
        double			audio_diff_threshold;
        double			audio_diff_avg_count;
        struct SwrContext *swr_ctx;

        SDL_Thread      *parse_tid;
        int             quit;
        int				av_sync_type;

        double			frame_timer;
        double			frame_last_pts;
        double			frame_last_delay;
        double			external_clock_base;
    } VideoState;
    
    static VideoState *global_video_state;
    static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;
    static int g_video_width, g_video_height;
    static char g_video_resized;
    static SDL_Surface *screen;
    static AVPacket flush_pkt;


    void packet_queue_init(PacketQueue *q);
    int packet_queue_put(PacketQueue *q, AVPacket *pkt);
    static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block);
    static void packet_queue_flush(PacketQueue *q);
    int audio_decode_frame(VideoState *is, double *pts_ptr);
    int queue_picture(VideoState *is, AVFrame *pFrame, double pts);
    static double get_video_clock(VideoState *is);
    static double get_audio_clock(VideoState *is);
    static double get_external_clock(VideoState *is);
    static double get_master_clock(VideoState *is);
    int synchronize_audio(VideoState *is, short *samples, int samples_size, double pts);
    double synchronize_video(VideoState *is, AVFrame *src_frame, double pts);
    void audio_callback(void *userdata, Uint8 *stream, int len);
    int video_thread(void *arg);
    int stream_component_open(VideoState *is, int stream_index);
    static int decode_interrupt_cb(void *arg);
    static int decode_thread(void *arg);
    void alloc_picture(void *userdata);
    static Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque);
    static void schedule_refresh(VideoState *is, int delay);
    void video_display(VideoState *is);
    void stream_seek(VideoState *is, int64_t pos, int rel);
    void video_refresh_timer(void *userdata);
    int play(const char* in_arg, Preview_type type);
#endif