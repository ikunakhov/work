#include <Audio.h>
#include <one_converter.h>


Audio_inf_err get_audio_info_qt(QString input_name)
{
    Audio_inf_err ret; ret.err = 0;
    AVFormatContext	*pFormatCtx = NULL;
    AVCodecContext	*pCodecCtx  = NULL;
    AVCodec			*pCodec     = NULL;
    int audioStream, i;
    av_register_all();

    QByteArray ba;
    ba = input_name.toLatin1();
    const char* filename; filename = ba.data();

    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
    {
        ret.err = -1;
        ret.txt_err = QString("Couldn't open input stream");
        return ret;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        ret.err = -1;
        ret.txt_err = QString("Couldn't find stream information");
        return ret;
    }
    audioStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            break;
        }

    if (audioStream == -1) {
        ret.err = -1;
        ret.txt_err = QString("Didn't find a audio stream");
        return ret;
    }
    pCodecCtx = pFormatCtx->streams[audioStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        ret.err = -1;
        ret.txt_err = QString("Codec not found");
        return ret;
    }
    ret.info.bitrate = QString::number(pCodecCtx->bit_rate / 1000);
    ret.info.duration = int_to_time(pFormatCtx->duration).toString();
    ret.info.format =  QString(pFormatCtx->iformat->name);
    ret.info.frequency = QString::number(pCodecCtx->sample_rate);
    ret.info.codec =  QString(avcodec_get_name(pCodec->id));
    ret.info.chanels = QString::number(av_get_channel_layout_nb_channels(pCodecCtx->channel_layout));
    return ret;

}

static Audio_c_err open_input_file(const char *filename)
{
    Audio_c_err ret; ret.err = 0;
    unsigned int i;

    ifmt_ctx = NULL;
    if ((ret.err = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
        ret.txt_err = QString("Cannot open input file");
        return ret;
    }

    if ((ret.err = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        ret.txt_err = QString("Cannot find stream information");
        return ret;
    }

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *stream;
        AVCodecContext *codec_ctx;
        stream = ifmt_ctx->streams[i];
        codec_ctx = stream->codec;
        if (codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            ret.err = avcodec_open2(codec_ctx, avcodec_find_decoder(codec_ctx->codec_id), NULL);
            if (ret.err < 0) {
                ret.txt_err = QString("Failed to open decoder for stream");
                return ret;
            }
        }
    }
    return ret;
}

static Audio_c_err open_output_file(const char *filename, Transcoding_context init_context)
{
    Audio_c_err ret; ret.err = 0;
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec *encoder = NULL;
    unsigned int i;
    AVCodecID AudioCodec_id = init_context.audio_context.codec_id;

    ofmt_ctx = NULL;
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if (!ofmt_ctx) {
        ret.err = -1;
        ret.txt_err = QString("Could not create output context");
        return ret;
    }


    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            ret.err = -1;
            ret.txt_err = QString("Failed allocating output stream");
            return ret;
        }

        in_stream = ifmt_ctx->streams[i];
        dec_ctx = in_stream->codec;
        enc_ctx = out_stream->codec;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
                encoder = avcodec_find_encoder(AudioCodec_id);

            if (!encoder) {
                ret.err = -1;
                ret.txt_err = QString("Necessary encoder not found");
                return ret;
            }
            enc_ctx->sample_rate = init_context.audio_context.sample_rate;
            enc_ctx->channel_layout = av_get_default_channel_layout(init_context.audio_context.chanel_layout);
            enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
            enc_ctx->sample_fmt = encoder->sample_fmts[0];
            out_stream->time_base.den = dec_ctx->sample_rate;
            out_stream->time_base.num = 1;
    
            (enc_ctx)->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            ret.err = avcodec_open2(enc_ctx, encoder, NULL);
            if (ret.err < 0) {
                ret.txt_err = QString("Cannot open video encoder for stream");
                return ret;
            }
        }
        else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            ret.err = -1;
            ret.txt_err = QString("Elementary stream is of unknown type, cannot proceed");
            return ret;
        }
        else {
            ret.err = avcodec_copy_context(ofmt_ctx->streams[i]->codec,
                ifmt_ctx->streams[i]->codec);
            if (ret.err < 0) {
                ret.txt_err = QString("Copying stream context failed\n");
                return ret;
            }
        }

        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    }

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret.err = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret.err < 0) {
            ret.txt_err =  QString("Could not open output file");
            return ret;
        }
    }

    ret.err = avformat_write_header(ofmt_ctx, NULL);
    if (ret.err < 0) {
        ret.txt_err = QString("Error occurred when opening output file");
        return ret;
    }

    return ret;
}

static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
    AVCodecContext *enc_ctx, const char *filter_spec)
{
    char args[512];
    int ret = 0;
    AVFilter *buffersrc = NULL;
    AVFilter *buffersink = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = avfilter_graph_alloc();

    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

 if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink = avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        if (!dec_ctx->channel_layout)
            dec_ctx->channel_layout =
            av_get_default_channel_layout(dec_ctx->channels);
        _snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64, dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate, av_get_sample_fmt_name(dec_ctx->sample_fmt),
            dec_ctx->channel_layout);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
            args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
            NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
            (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
            AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "channel_layouts",
            (uint8_t*)&enc_ctx->channel_layout,
            sizeof(enc_ctx->channel_layout), AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
            (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
            AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
            goto end;
        }
    }
    else {
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    if (!outputs->name || !inputs->name) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
        &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph = filter_graph;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static Audio_c_err init_filters(void)
{
    Audio_c_err ret; ret.err = 0;
    const char *filter_spec;
    unsigned int i;
 
    filter_ctx = (FilteringContext*)av_malloc_array(ifmt_ctx->nb_streams, sizeof(*filter_ctx));
    if (!filter_ctx) {
        ret.err = -1;
        ret.txt_err = QString("Can not allocane memory for filtering context");
        return ret;
    }

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        filter_ctx[i].buffersrc_ctx = NULL;
        filter_ctx[i].buffersink_ctx = NULL;
        filter_ctx[i].filter_graph = NULL;
        if (!(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO || ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;


        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            filter_spec = "null"; 
        else
            filter_spec = "anull"; 
        ret.err = init_filter(&filter_ctx[i], ifmt_ctx->streams[i]->codec,
            ofmt_ctx->streams[i]->codec, filter_spec);
        if (ret.err)
            return ret;
    }
    return ret;
}

static int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame) {
    int ret;
    int got_frame_local;
    AVPacket enc_pkt;
    int(*enc_func)(AVCodecContext *, AVPacket *, const AVFrame *, int *) =
        (ifmt_ctx->streams[stream_index]->codec->codec_type ==
            AVMEDIA_TYPE_VIDEO) ? avcodec_encode_video2 : avcodec_encode_audio2;

    if (!got_frame)
        got_frame = &got_frame_local;

    av_log(NULL, AV_LOG_INFO, "Encoding frame\n");

    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);
    ret = enc_func(ofmt_ctx->streams[stream_index]->codec, &enc_pkt,
        filt_frame, got_frame);
    av_frame_free(&filt_frame);
    if (ret < 0)
        return ret;
    if (!(*got_frame))
        return 0;

 
    enc_pkt.stream_index = stream_index;
    av_packet_rescale_ts(&enc_pkt,
        ofmt_ctx->streams[stream_index]->codec->time_base,
        ofmt_ctx->streams[stream_index]->time_base);

    av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");

    ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
    return ret;
}

static int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index)
{
    int ret;
    AVFrame *filt_frame;

    av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
 
    ret = av_buffersrc_add_frame_flags(filter_ctx[stream_index].buffersrc_ctx,
        frame, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }

    while (1) {
        filt_frame = av_frame_alloc();
        if (!filt_frame) {
            ret = AVERROR(ENOMEM);
            break;
        }
        av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(filter_ctx[stream_index].buffersink_ctx,
            filt_frame);
        if (ret < 0) {

            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            av_frame_free(&filt_frame);
            break;
        }

        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
        ret = encode_write_frame(filt_frame, stream_index, NULL);
        if (ret < 0)
            break;
    }

    return ret;
}

static int flush_encoder(unsigned int stream_index)
{
    int ret;
    int got_frame;

    if (!(ofmt_ctx->streams[stream_index]->codec->codec->capabilities & AV_CODEC_CAP_DELAY))
        return 0;

    while (1) {
        av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(NULL, stream_index, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}

Audio_c_err Ui_ONE_conv::transcoding_audio(QString in_file, QString ou_file, Transcoding_context input_context)
{
    progressBar->setValue(0);
    Loading_widget->show();

    label_2->setText("OPENINIG INPUT FILE...");
    Audio_c_err ret; ret.err = 0;
    AVPacket packet; packet.data = NULL; packet.size = 0;
    AVFrame *frame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    int(*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);

    av_register_all();
    avfilter_register_all();
    avcodec_register_all();

    QByteArray ba1;
    ba1 = in_file.toLatin1();
    const char* file_in; file_in = ba1.data();
    QByteArray ba2;
    ba2 = ou_file.toLatin1();
    const char* file_out; file_out = ba2.data();

    ret = open_input_file(file_in);
    if (ret.err < 0) return ret;
    label_23->setText("OPENINIG OUTPUT FILE...");
    ret = open_output_file(file_out, input_context);
    if (ret.err < 0) return ret;
    label_23->setText("FILTERS INITIALIZATION...");
    ret = init_filters();
    if (ret.err < 0) return ret;

    int k = 0;
    int size_coeff = ifmt_ctx->duration / sizeof(packet);
    label_23->setText("TRANSCODING AUDIO...");
    while (1) {
        if ((ret.err = av_read_frame(ifmt_ctx, &packet)) < 0)
            break;
        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[packet.stream_index]->codec->codec_type;
        progressBar->setValue(int(k*30100 / size_coeff));
        k++;

        if (filter_ctx[stream_index].filter_graph) {
            frame = av_frame_alloc();
            if (!frame) {
                ret.err = -1;
                ret.txt_err = QString("Can not allocate frame");
                return ret;
            }
            av_packet_rescale_ts(&packet, ifmt_ctx->streams[stream_index]->time_base, ifmt_ctx->streams[stream_index]->codec->time_base);
            dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 : avcodec_decode_audio4;
            ret.err = dec_func(ifmt_ctx->streams[stream_index]->codec, frame, &got_frame, &packet);
            if (ret.err < 0) {
                av_frame_free(&frame);
                ret.txt_err = QString("Error in decoding function");
                return ret;
            }

            if (got_frame) {
                frame->pts = av_frame_get_best_effort_timestamp(frame);
                ret.err = filter_encode_write_frame(frame, stream_index);
                av_frame_free(&frame);
                if (ret.err < 0) {
                    ret.txt_err = QString("Error infiltering encode frame");
                    return ret;
                }
            }
            else {
                av_frame_free(&frame);
            }
        }
        else {
         
            av_packet_rescale_ts(&packet,
                ifmt_ctx->streams[stream_index]->time_base,
                ofmt_ctx->streams[stream_index]->time_base);

            ret.err = av_interleaved_write_frame(ofmt_ctx, &packet);
            if (ret.err < 0) {
                ret.txt_err = QString("Error in writing frame");
                return ret;
            }
        }
        av_packet_unref(&packet);
    }
    progressBar->setValue(99);

 
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
       
        if (!filter_ctx[i].filter_graph)
            continue;
        ret.err = filter_encode_write_frame(NULL, i);
        if (ret.err < 0) {
            ret.txt_err = QString("Flushing filter failed\n");
            return ret;
        }

       
        ret.err = flush_encoder(i);
        if (ret.err < 0) {
            ret.txt_err = QString("Flushing encoder failed\n");
            return ret;
        }
    }

    progressBar->setValue(100);
    av_write_trailer(ofmt_ctx);
    av_packet_unref(&packet);
    av_frame_free(&frame);
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        avcodec_close(ifmt_ctx->streams[i]->codec);
        if (ofmt_ctx && ofmt_ctx->nb_streams > i && ofmt_ctx->streams[i] && ofmt_ctx->streams[i]->codec)
            avcodec_close(ofmt_ctx->streams[i]->codec);
        if (filter_ctx && filter_ctx[i].filter_graph)
            avfilter_graph_free(&filter_ctx[i].filter_graph);
    }
    av_free(filter_ctx);
    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    if (ret.err < 0)
        return ret;
    return ret;
}

AVCodecID qstring_to_sup_codec_id(QString codec_name)
{
    if (codec_name == QString("AAC"))       return AV_CODEC_ID_AAC;
    if (codec_name == QString("MP2"))       return AV_CODEC_ID_MP2;
    if (codec_name == QString("MP3"))       return AV_CODEC_ID_MP3;
    if (codec_name == QString("AC3"))       return AV_CODEC_ID_AC3;
    if (codec_name == QString("FLAC"))      return AV_CODEC_ID_FLAC;
    if (codec_name == QString("DTS"))       return AV_CODEC_ID_DTS;
    if (codec_name == QString("WAVPACK"))   return AV_CODEC_ID_WAVPACK;
    
    return AV_CODEC_ID_NONE;
}

    QTime int_to_time(int time_in)
    {
        int real_sec = time_in / 1000000;
        int h = real_sec / 3600;
        int m = real_sec / 60 - h * 60;
        int s = real_sec - m * 60 - h * 3600;
        QTime dur(h, m, s);
        return dur;
    }
