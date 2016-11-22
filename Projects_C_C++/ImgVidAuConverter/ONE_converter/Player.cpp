#include <Player.h>
    void packet_queue_init(PacketQueue *q) {
        memset(q, 0, sizeof(PacketQueue));
        q->mutex = SDL_CreateMutex();
        q->cond = SDL_CreateCond();
    }

    int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
        AVPacketList* pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
        if (pkt1 == NULL) {
            return -1;
        }
        if (av_packet_ref(&pkt1->pkt, pkt) < 0) {
            return -1;
        }
        else {
            pkt1->pkt = *pkt;

        }
        pkt1->next = NULL;

        SDL_LockMutex(q->mutex);

        if (!q->last_pkt) {
            q->first_pkt = pkt1;
        }
        else {
            q->last_pkt->next = pkt1;
        }

        q->last_pkt = pkt1;
        q->nb_packets++;
        q->size += pkt1->pkt.size;
        SDL_CondSignal(q->cond);
        SDL_UnlockMutex(q->mutex);
        return 0;
    }

    static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
        AVPacketList *pkt1;
        int ret;

        SDL_LockMutex(q->mutex);

        for (;;) {
            if (global_video_state->quit) {
                ret = -1;
                break;
            }

            pkt1 = q->first_pkt;
            if (pkt1) {
                q->first_pkt = pkt1->next;
                if (!q->first_pkt) {
                    q->last_pkt = NULL;
                }
                q->nb_packets--;
                q->size -= pkt1->pkt.size;
                *pkt = pkt1->pkt;

                av_free(pkt1);
                ret = 1;
                break;
            }
            else if (!block) {
                ret = 0;
                break;
            }
            else {
                SDL_CondWait(q->cond, q->mutex);
            }
        }

        SDL_UnlockMutex(q->mutex);

        return ret;
    }

    static void packet_queue_flush(PacketQueue *q) {
        AVPacketList *pkt, *pkt1;

        SDL_LockMutex(q->mutex);

        for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
            pkt1 = pkt->next;
            av_packet_unref(&pkt->pkt);
            av_freep(&pkt);
        }

        q->last_pkt = NULL;
        q->first_pkt = NULL;
        q->nb_packets = 0;
        q->size = 0;

        SDL_UnlockMutex(q->mutex);
    }

    int audio_decode_frame(VideoState *is, double *pts_ptr) {
        int len1, len2, decoded_data_size;
        AVPacket *pkt = &is->audio_pkt;
        int got_frame = 0;
        int64_t dec_channel_layout;
        int wanted_nb_samples, resampled_data_size;
        int flush_complete = 0;
        double pts = 0;

        for (;;) {
            while (is->audio_pkt_size > 0) {
                if (!is->audio_frame) {
                    if (!(is->audio_frame = av_frame_alloc())) {
                        return AVERROR(ENOMEM);
                    }
                }
                else
                    is->audio_frame = av_frame_alloc();

                if (flush_complete) {
                    break;
                }

                len1 = avcodec_decode_audio4(is->audio_st->codec, is->audio_frame, &got_frame, pkt);
                if (len1 < 0) {
                    is->audio_pkt_size = 0;
                    break;
                }

                is->audio_pkt_data += len1;
                is->audio_pkt_size -= len1;

                if (!got_frame) {
                    flush_complete = 1;
                    continue;
                }

                decoded_data_size = av_samples_get_buffer_size(NULL, is->audio_frame->channels, is->audio_frame->nb_samples, (AVSampleFormat)is->audio_frame->format, 1);

                dec_channel_layout = (is->audio_frame->channel_layout && is->audio_frame->channels
                    == av_get_channel_layout_nb_channels(is->audio_frame->channel_layout))
                    ? is->audio_frame->channel_layout
                    : av_get_default_channel_layout(is->audio_frame->channels);

                wanted_nb_samples = is->audio_frame->nb_samples;

                if (is->audio_frame->format != is->audio_src_fmt ||
                    dec_channel_layout != is->audio_src_channel_layout ||
                    is->audio_frame->sample_rate != is->audio_src_freq ||
                    (wanted_nb_samples != is->audio_frame->nb_samples && !is->swr_ctx)) {
                    if (is->swr_ctx) swr_free(&is->swr_ctx);
                    is->swr_ctx = swr_alloc_set_opts(NULL,
                        is->audio_tgt_channel_layout,
                        is->audio_tgt_fmt,
                        is->audio_tgt_freq,
                        dec_channel_layout,
                        (AVSampleFormat)is->audio_frame->format,
                        is->audio_frame->sample_rate,
                        0, NULL);
                    if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
                        //"swr_init() failed\n"
                        break;
                    }
                    is->audio_src_channel_layout = dec_channel_layout;
                    is->audio_src_channels = is->audio_st->codec->channels;
                    is->audio_src_freq = is->audio_st->codec->sample_rate;
                    is->audio_src_fmt = is->audio_st->codec->sample_fmt;
                }
                if (is->swr_ctx) {
                    const uint8_t **in = (const uint8_t **)is->audio_frame->extended_data;
                    uint8_t *out[] = { is->audio_buf2 };

                    if (wanted_nb_samples != is->audio_frame->nb_samples) {
                        if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - is->audio_frame->nb_samples)
                            * is->audio_tgt_freq / is->audio_frame->sample_rate,
                            wanted_nb_samples * is->audio_tgt_freq / is->audio_frame->sample_rate) < 0) {
                            //"swr_set_compensation() failed\n"
                            break;
                        }
                    }

                    len2 = swr_convert(is->swr_ctx, out,
                        sizeof(is->audio_buf2) / is->audio_tgt_channels / av_get_bytes_per_sample(is->audio_tgt_fmt),
                        in, is->audio_frame->nb_samples);

                    if (len2 < 0) {
                        //"swr_convert() failed\n"
                        break;
                    }
                    if (len2 == sizeof(is->audio_buf2) / is->audio_tgt_channels / av_get_bytes_per_sample(is->audio_tgt_fmt)) {
                        //"warning: audio buffer is probably too small\n"
                        swr_init(is->swr_ctx);
                    }
                    is->audio_buf = is->audio_buf2;
                    resampled_data_size = len2 * is->audio_tgt_channels * av_get_bytes_per_sample(is->audio_tgt_fmt);
                }
                else {
                    resampled_data_size = decoded_data_size;
                    is->audio_buf = is->audio_frame->data[0];
                }
                pts = is->audio_clock;
                *pts_ptr = pts;
                is->audio_clock += (double)decoded_data_size / (is->audio_st->codec->channels * is->audio_st->codec->sample_rate * av_get_bytes_per_sample(is->audio_st->codec->sample_fmt));

                return resampled_data_size;
            }

            if (pkt->data) av_packet_unref(pkt);
            memset(pkt, 0, sizeof(*pkt));
            if (is->quit)
                return -1;

            if (packet_queue_get(&is->audioq, pkt, 1) < 0)
                return -1;

            if (pkt->data == flush_pkt.data) {
                avcodec_flush_buffers(is->audio_st->codec);
                continue;
            }

            is->audio_pkt_data = pkt->data;
            is->audio_pkt_size = pkt->size;
            if (pkt->pts != AV_NOPTS_VALUE)
                is->audio_clock = av_q2d(is->audio_st->codec->time_base) * pkt->pts;
        }
    }

    int queue_picture(VideoState *is, AVFrame *pFrame, double pts) {
        VideoPicture *vp;
        AVPixelFormat dst_pix_fmt;
        AVFrame *pict; pict = av_frame_alloc();
        static struct SwsContext *img_convert_ctx;

        SDL_LockMutex(is->pictq_mutex);
        while (is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE && !is->quit) {
            SDL_CondWait(is->pictq_cond, is->pictq_mutex);
        }
        SDL_UnlockMutex(is->pictq_mutex);

        if (is->quit) return -1;
        vp = &is->pictq[is->pictq_windex];
        if (!vp->bmp || vp->width != is->video_st->codec->width || vp->height != is->video_st->codec->height || g_video_resized) {
            SDL_Event event;
            vp->allocated = 0;
            event.type = FF_ALLOC_EVENT;
            event.user.data1 = is;
            SDL_PushEvent(&event);
            SDL_LockMutex(is->pictq_mutex);
            while (!vp->allocated && !is->quit) {
                SDL_CondWait(is->pictq_cond, is->pictq_mutex);
            }
            SDL_UnlockMutex(is->pictq_mutex);
            if (is->quit) return -1;
        }
        if (vp->bmp) {
            SDL_LockYUVOverlay(vp->bmp);
            dst_pix_fmt = AV_PIX_FMT_YUV420P;
            pict->data[0] = vp->bmp->pixels[0];
            pict->data[1] = vp->bmp->pixels[2];
            pict->data[2] = vp->bmp->pixels[1];

            pict->linesize[0] = vp->bmp->pitches[0];
            pict->linesize[1] = vp->bmp->pitches[2];
            pict->linesize[2] = vp->bmp->pitches[1];

            if (img_convert_ctx == NULL) {
                int w = is->video_st->codec->width;
                int h = is->video_st->codec->height;
                img_convert_ctx = sws_getContext(w, h, is->video_st->codec->pix_fmt,
                    w, h, dst_pix_fmt, SWS_BICUBIC,
                    NULL, NULL, NULL);
                if (img_convert_ctx == NULL) {
                    //"Connot initialize the convertion context!\n"
                    exit(1);
                }
            }
            sws_scale(img_convert_ctx, (const uint8_t**)pFrame->data, pFrame->linesize,
                0, is->video_st->codec->height, pict->data, pict->linesize);
            SDL_UnlockYUVOverlay(vp->bmp);
            vp->pts = pts;
            if (++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) is->pictq_windex = 0;
            SDL_LockMutex(is->pictq_mutex);
            is->pictq_size++;
            SDL_UnlockMutex(is->pictq_mutex);
        }
        return 0;
    }

    static double get_video_clock(VideoState *is) {
        double delta;

        delta = (av_gettime() - is->video_current_pts_time) / 1000000.0;
        return is->video_current_pts + delta;
    }

    static double get_audio_clock(VideoState *is) {
        double pts;
        int hw_buf_size, bytes_per_sec, n;

        pts = is->audio_clock;
        hw_buf_size = is->audio_buf_size - is->audio_buf_index;
        bytes_per_sec = 0;
        if (is->audio_st) {
            n = is->audio_st->codec->channels * 2;
            bytes_per_sec = is->audio_st->codec->sample_rate * n;
        }
        if (bytes_per_sec) {
            pts -= (double)hw_buf_size / bytes_per_sec;
        }
        return pts;
    }

    static double get_external_clock(VideoState *is) {
        return (av_gettime() / 1000000.0) - is->external_clock_base;
    }

    static double get_master_clock(VideoState *is) {
        if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
            return get_video_clock(is);
        }
        else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
            return get_audio_clock(is);
        }
        else {
            return get_external_clock(is);
        }
    }

    int synchronize_audio(VideoState *is, short *samples, int samples_size, double pts) {
        int n;
        double ref_clock;
        n = 2 * is->audio_st->codec->channels;
        if (is->av_sync_type != AV_SYNC_AUDIO_MASTER) {
            double diff, avg_diff;
            int wanted_size, min_size, max_size, nb_samples;
            ref_clock = get_master_clock(is);
            diff = get_audio_clock(is) - ref_clock;
            if (diff < AV_NOSYNC_THRESHOLD) {
                is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
                if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
                    is->audio_diff_avg_count++;
                }
                else {
                    avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);
                    if (fabs(avg_diff) >= is->audio_diff_threshold) {
                        wanted_size = samples_size + ((int)(diff * is->audio_st->codec->sample_rate) * n);
                        min_size = samples_size * ((100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100);
                        max_size = samples_size * ((100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100);
                        if (wanted_size < min_size) {
                            wanted_size = min_size;
                        }
                        else if (wanted_size > max_size) {
                            wanted_size = max_size;
                        }
                        if (wanted_size < samples_size) {
                            samples_size = wanted_size;
                        }
                        else if (wanted_size > samples_size) {
                            uint8_t *samples_end, *q;
                            int nb;

                            nb = (samples_size - wanted_size);
                            samples_end = (uint8_t*)samples + samples_size - n;
                            q = samples_end + n;
                            while (nb > 0) {
                                memcpy(q, samples_end, n);
                                q += n;
                                nb -= n;
                            }
                            samples_size = wanted_size;
                        }
                    }
                }
            }
            else {
                is->audio_diff_avg_count = 0;
                is->audio_diff_cum = 0;
            }
        }
        return samples_size;
    }

    double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) {
        double frame_delay;

        if (pts != 0) {
            is->video_clock = pts;
        }
        else {
            pts = is->video_clock;
        }

        frame_delay = av_q2d(is->video_st->codec->time_base);
        frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
        is->video_clock += frame_delay;
        return pts;
    }

    void audio_callback(void *userdata, Uint8 *stream, int len) {
        VideoState *is = (VideoState *)userdata;
        int len1, audio_data_size;
        double pts;

        while (len > 0) {
            if (is->audio_buf_index >= is->audio_buf_size) {
                audio_data_size = audio_decode_frame(is, &pts);

                if (audio_data_size < 0) {

                    is->audio_buf_size = 1024;
                    memset(is->audio_buf, 0, is->audio_buf_size);
                }
                else {
                    is->audio_buf_size = audio_data_size;
                }
                is->audio_buf_index = 0;
            }

            len1 = is->audio_buf_size - is->audio_buf_index;
            if (len1 > len) {
                len1 = len;
            }

            memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
            len -= len1;
            stream += len1;
            is->audio_buf_index += len1;
        }
    }

    int video_thread(void *arg) {
        VideoState *is = (VideoState*)arg;
        AVPacket pkt1, *packet = &pkt1;
        int len1, frameFinished;
        AVFrame *pFrame;
        double pts;

        pFrame = av_frame_alloc();

        for (;;) {
            if (packet_queue_get(&is->videoq, packet, 1) < 0) {
                //"%d: packet_queue_get errror\n"
                break;
            }

            if (packet->data == flush_pkt.data) {
                avcodec_flush_buffers(is->video_st->codec);
                continue;
            }

            pts = 0;
            global_video_pkt_pts = packet->pts;

            len1 = avcodec_decode_video2(is->video_st->codec, pFrame, &frameFinished, packet);

            if (frameFinished) {
                pts = synchronize_video(is, pFrame, pts);
                if (queue_picture(is, pFrame, pts) < 0)
                    break;
            }
            av_packet_unref(packet);
        }
        av_free(pFrame);
        return 0;
    }

    int stream_component_open(VideoState *is, int stream_index) {
        AVFormatContext *ic = is->ic;
        AVCodecContext *codecCtx;
        AVCodec *codec;
        SDL_AudioSpec wanted_spec, spec;
        int64_t wanted_channel_layout = 0;
        int wanted_nb_channels;
        const int next_nb_channels[] = { 0, 0, 1 ,6, 2, 6, 4, 6 };

        if (stream_index < 0 || stream_index >= ic->nb_streams) {
            return -1;
        }

        codecCtx = ic->streams[stream_index]->codec;

        if (codecCtx->codec_type == AVMEDIA_TYPE_AUDIO) {
            wanted_nb_channels = codecCtx->channels;
            if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
                wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
                wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
            }

            wanted_spec.channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
            wanted_spec.freq = codecCtx->sample_rate;
            if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
               
                return -1;
            }
        }

        if (codecCtx->codec_type == AVMEDIA_TYPE_AUDIO) {
            wanted_spec.format = AUDIO_S16SYS;
            wanted_spec.silence = 0;
            wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
            wanted_spec.callback = audio_callback;
            wanted_spec.userdata = is;

            while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
                
                wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
                if (!wanted_spec.channels) {
                    
                    return -1;
                }
                wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
            }

            if (spec.format != AUDIO_S16SYS) {
                
                return -1;
            }
            if (spec.channels != wanted_spec.channels) {
                wanted_channel_layout = av_get_default_channel_layout(spec.channels);
                if (!wanted_channel_layout) {
                  
                    return -1;
                }
            }

            is->audio_src_fmt = is->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
            is->audio_src_freq = is->audio_tgt_freq = spec.freq;
            is->audio_src_channel_layout = is->audio_tgt_channel_layout = wanted_channel_layout;
            is->audio_src_channels = is->audio_tgt_channels = spec.channels;
        }

        codec = avcodec_find_decoder(codecCtx->codec_id);
        if (!codec || (avcodec_open2(codecCtx, codec, NULL) < 0)) {
            
            return -1;
        }


        switch (codecCtx->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            is->audioStream = stream_index;
            is->audio_st = ic->streams[stream_index];
            is->audio_buf_size = 0;
            is->audio_buf_index = 0;
            memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
            packet_queue_init(&is->audioq);
            SDL_PauseAudio(0);
            break;
        case AVMEDIA_TYPE_VIDEO:
            is->videoStream = stream_index;
            is->video_st = ic->streams[stream_index];
            is->frame_timer = (double)av_gettime() / 1000000.0;
            is->frame_last_delay = 40e-3;
            is->video_current_pts_time = av_gettime();
            packet_queue_init(&is->videoq);
            is->video_tid = SDL_CreateThread(video_thread, is);
        default:
            break;
        }
    }


    static int decode_interrupt_cb(void *arg) {
        return (global_video_state && global_video_state->quit);
    }

    static int decode_thread(void *arg) {
        VideoState *is = (VideoState *)arg;
        AVFormatContext *ic = NULL;
        AVPacket pkt1, *packet = &pkt1;
        int ret, i;
        int video_index = -1;
        int audio_index = -1;

        is->audioStream = -1;
        is->videoStream = -1;

        global_video_state = is;
        if (avformat_open_input(&ic, is->filename, NULL, NULL) != 0) {
            return -1;
        }

        static const AVIOInterruptCB int_cb = { decode_interrupt_cb, NULL };
        ic->interrupt_callback = int_cb;

        is->ic = ic;
        is->external_clock_base = 0;
        is->external_clock_base = get_external_clock(is);

        if (avformat_find_stream_info(ic, NULL) < 0) {
            return -1;
        }

        av_dump_format(ic, 0, is->filename, 0);

        for (i = 0; i < ic->nb_streams; i++) {
            if (ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audio_index < 0)
                audio_index = i;
            if (ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && video_index < 0)
                video_index = i;
        }

        if (video_index >= 0) stream_component_open(is, video_index);

        if (audio_index >= 0) stream_component_open(is, audio_index);

        for (;;) {
            if (is->quit)
                break;

            if (is->seek_req) {
                int stream_index = -1;
                int64_t seek_target = is->seek_pos;

                is->external_clock_base = 0;
                is->external_clock_base = get_external_clock(is) - (seek_target / 1000000.0);

                if (is->videoStream >= 0)
                    stream_index = is->videoStream;
                else if (is->audioStream >= 0)
                    stream_index = is->audioStream;


                if (stream_index >= 0)

                    if (av_seek_frame(is->ic, stream_index, seek_target, is->seek_flags) < 0) {
                        
                    }
                    else {

                        if (is->audioStream >= 0) {
                            packet_queue_flush(&is->audioq);
                            packet_queue_put(&is->audioq, &flush_pkt);
                        }
                        if (is->videoStream >= 0) {
                            packet_queue_flush(&is->videoq);
                            packet_queue_put(&is->videoq, &flush_pkt);
                        }
                    }

                    is->seek_req = 0;
            }

            if (is->audioq.size > MAX_AUDIOQ_SIZE || is->videoq.size > MAX_VIDEOQ_SIZE) {
                SDL_Delay(100);
                continue;
            }

            ret = av_read_frame(is->ic, packet);
            if (ret < 0) {
                if (ret == AVERROR_EOF || avio_feof(is->ic->pb)) {
                    break;
                }
                if (is->ic->pb && is->ic->pb->error) {
                    break;
                }
                continue;
            }

            if (packet->stream_index == is->videoStream) {
                packet_queue_put(&is->videoq, packet);
            }
            else if (packet->stream_index == is->audioStream) {
                packet_queue_put(&is->audioq, packet);
            }
            else {
                av_packet_unref(packet);
            }
        }

        while (!is->quit) {
            SDL_Delay(100);
        }

    fail: {
        SDL_Event event;
        event.type = FF_QUIT_EVENT;
        event.user.data1 = is;
        SDL_PushEvent(&event);
        }

          return 0;
    }

    void alloc_picture(void *userdata) {
        VideoState *is = (VideoState*)userdata;
        VideoPicture *vp;

        vp = &is->pictq[is->pictq_windex];
        if (vp->bmp) {
            SDL_FreeYUVOverlay(vp->bmp);
        }

        if (g_video_resized) {
            screen = NULL;
            screen = SDL_SetVideoMode(g_video_width, g_video_height, SDL_VIDEO_MODE_BPP, SDL_VIDEO_MODE_FLAGS);
            g_video_resized = 0;
        }
        vp->bmp = SDL_CreateYUVOverlay(is->video_st->codec->width, is->video_st->codec->height, SDL_YV12_OVERLAY, screen);

        vp->width = is->video_st->codec->width;
        vp->height = is->video_st->codec->height;

        SDL_LockMutex(is->pictq_mutex);
        vp->allocated = 1;
        SDL_CondSignal(is->pictq_cond);
        SDL_UnlockMutex(is->pictq_mutex);
    }

    static Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque) {
        SDL_Event event;
        event.type = FF_REFRESH_EVENT;
        event.user.data1 = opaque;
        SDL_PushEvent(&event);

        return 0;
    }

    static void schedule_refresh(VideoState *is, int delay) {
        SDL_AddTimer(delay, sdl_refresh_timer_cb, is);
    }

    void video_display(VideoState *is) {
        SDL_Rect rect;
        VideoPicture *vp;
        AVPicture pict;
        float aspect_ratio;
        int w, h, x, y;
        int i;

        vp = &is->pictq[is->pictq_rindex];
        if (vp->bmp) {
            if (is->video_st->codec->sample_aspect_ratio.num == 0) {
                aspect_ratio = 0;
            }
            else {
                aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio) *
                    is->video_st->codec->width / is->video_st->codec->height;
            }
            if (aspect_ratio <= 0.0)
                aspect_ratio = (float)is->video_st->codec->width / (float)is->video_st->codec->height;

            h = screen->h;
            w = ((int)(h * aspect_ratio)) & -3;
            if (w > screen->w) {
                w = screen->w;
                h = ((int)(w / aspect_ratio)) & -3;
            }
            x = (screen->w - w) / 2;
            y = (screen->h - h) / 2;

            rect.x = x;
            rect.y = y;
            rect.w = w;
            rect.h = h;

            SDL_DisplayYUVOverlay(vp->bmp, &rect);
        }
    }

    void stream_seek(VideoState *is, int64_t pos, int rel) {
        if (!is->seek_req) {
            is->seek_pos = pos;
            is->seek_flags = rel < 0 ? AVSEEK_FLAG_BACKWARD : 0;
            is->seek_req = 1;
        }
    }

    void video_refresh_timer(void *userdata) {
        VideoState *is = (VideoState*)userdata;
        VideoPicture *vp;
        double actual_delay, delay, sync_threshold, ref_clock, diff;

        if (is->video_st) {
            if (is->pictq_size == 0) {
                schedule_refresh(is, 1);
            }
            else {
                vp = &is->pictq[is->pictq_rindex];
                is->video_current_pts = vp->pts;
                is->video_current_pts_time = av_gettime();
                delay = vp->pts - is->frame_last_pts;
                if (delay <= 0 || delay >= 1.0) delay = is->frame_last_delay;
                is->frame_last_delay = delay;
                is->frame_last_pts = vp->pts;
                if (is->av_sync_type != AV_SYNC_VIDEO_MASTER) {
                    ref_clock = get_master_clock(is);
                    diff = vp->pts - ref_clock;
                    sync_threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_SYNC_THRESHOLD;
                    if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
                        if (diff <= -sync_threshold) {
                            delay = 0;
                        }
                        else if (diff >= sync_threshold) {
                            delay = 2 * delay;
                        }
                    }
                }
                is->frame_timer += delay;
                actual_delay = is->frame_timer - (av_gettime() / 1000000.0);
                if (actual_delay < 0.010) actual_delay = 0.010;
                schedule_refresh(is, (int)(actual_delay * 1000 + 0.5));
                video_display(is);
                if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) is->pictq_rindex = 0;
                SDL_LockMutex(is->pictq_mutex);
                is->pictq_size--;
                SDL_CondSignal(is->pictq_cond);
                SDL_UnlockMutex(is->pictq_mutex);
            }
        }
        else
            schedule_refresh(is, 10);
    }

    int play(const char* in_agr, Preview_type type)
    {
        SDL_Event       event;
        double			pos;
        VideoState      *is;

        is = (VideoState *)av_mallocz(sizeof(VideoState));

        avcodec_register_all();
        av_register_all();
        avformat_network_init();

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
          
            exit(1);
        }
        if (type == PLAYER_RUN || type == VIDEO_PREVIEW)
        {
            g_video_width = 640;
            g_video_height = 480;
            g_video_resized = 0;
            screen = SDL_SetVideoMode(g_video_width, g_video_height, SDL_VIDEO_MODE_BPP, SDL_VIDEO_MODE_FLAGS);
            if (!screen) {
                
                exit(1);
            }
        }
        if (type == AUDIO_PREVIEW)
        {
            g_video_width = 1;
            g_video_height = 1;
            g_video_resized = 0;
            screen = SDL_SetVideoMode(g_video_width, g_video_height, SDL_VIDEO_MODE_BPP, SDL_NOFRAME);
            if (!screen) {
        
                exit(1);
            }
        }

        av_strlcpy(is->filename, in_agr, sizeof(is->filename));

        is->pictq_mutex = SDL_CreateMutex();
        is->pictq_cond = SDL_CreateCond();

        schedule_refresh(is, 40);

        is->av_sync_type = AV_SYNC_VIDEO_MASTER;

        is->parse_tid = SDL_CreateThread(decode_thread, is);
        if (!is->parse_tid) {
            av_free(is);
            return -1;
        }

        av_init_packet(&flush_pkt);
        flush_pkt.data = (uint8_t*)"FLUSH";

        for (;;) {
            double incr, pos;

            SDL_WaitEvent(&event);
            switch (event.type) {
            case SDL_VIDEORESIZE:
                g_video_width = event.resize.w;
                g_video_height = event.resize.h;
                g_video_resized = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    incr = -1.0;
                    goto do_seek;
                case SDLK_RIGHT:
                    incr = 1.0;
                    goto do_seek;
                case SDLK_UP:
                    incr = 6.0;
                    goto do_seek;
                case SDLK_DOWN:
                    incr = -6.0;
                    goto do_seek;
                case  SDLK_ESCAPE:
                    is->quit = 1;
                    SDL_Quit();
                    return 0;
                do_seek:
                    if (global_video_state) {

                        pos = get_master_clock(global_video_state);
                        pos += incr;
                        stream_seek(global_video_state, (int64_t)(pos * AV_TIME_BASE), incr);
                    }
                default:
                    break;
                }
                break;
            case FF_QUIT_EVENT:
            case SDL_QUIT:
                is->quit = 1;
                SDL_Quit();
                return 0;
                break;
            case FF_ALLOC_EVENT:
                alloc_picture(event.user.data1);
                break;
            case FF_REFRESH_EVENT:
                video_refresh_timer(event.user.data1);
            default:
                break;
            }
        }
        return 0;
    }