#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

static void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, FILE* outfile)
{
    int ret;

    if (frame)
        printf("Send frame %3lld\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) 
    {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) 
    {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) 
        {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3lld (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

int TestEncode264()
{
    const char* filename, * codec_name;
    const AVCodec* codec;
    AVCodecContext* c = NULL;
    int i, ret;
    FILE* f;
    AVFrame* frame;
    AVPacket* pkt;

    filename = "white.h264";
    codec_name = "libx264";

    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) 
    {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) 
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    c->bit_rate = 2000000;
    c->width = 1920;
    c->height = 1080;
    c->time_base = AVRational{ 1, 25 };
    c->framerate = AVRational{ 25, 1 };


    c->gop_size = 1;
    c->max_b_frames = 0;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "wb");
    if (!f) 
    {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) 
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    //ret = av_frame_get_buffer(frame, 0);
    //if (ret < 0) {
    //    fprintf(stderr, "Could not allocate the video frame data\n");
    //    exit(1);
    //}

    std::vector<uint8_t> yBuf;
    yBuf.resize((size_t)c->width * c->height, 235);
    std::vector<uint8_t> uBuf;
    uBuf.resize((size_t)c->width * c->height / 4, 128);
    std::vector<uint8_t> vBuf;
    vBuf.resize((size_t)c->width * c->height / 4, 128);

    for (i = 0; i < 25; i++)
    {
        //ret = av_frame_make_writable(frame);
        //if (ret < 0)
        //    exit(1);

        frame->data[0] = yBuf.data();
        frame->data[1] = uBuf.data();
        frame->data[2] = vBuf.data();
        frame->linesize[0] = c->width;
        frame->linesize[1] = c->width / 2;
        frame->linesize[2] = c->width / 2;

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, f);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, f);


    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}
