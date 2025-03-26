#pragma once

extern "C" {
#include <libavutil/error.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libavutil/pixdesc.h>
}
#include <string>
#include <iostream>

inline std::string GetFFmpegError(int errnum) {
    char errbuf[1024]{}; // 增加缓冲区大小
    av_strerror(errnum, errbuf, sizeof(errbuf));
    return std::string(errbuf);
}

inline void printCodecInfo(AVCodecID codec_id) {
    // 查找编解码器
    const AVCodec* codec = avcodec_find_decoder(codec_id);
    if (!codec) {
        codec = avcodec_find_encoder(codec_id);
    }

    if (codec) {
        std::cout << "Codec: " << codec->name << std::endl;
        std::cout << "Long Name: " << codec->long_name << std::endl;
        std::cout << "Type: " << av_get_media_type_string(codec->type) << std::endl;
        std::cout << "ID: " << codec->id << std::endl;
        std::cout << "Capabilities: " << codec->capabilities << std::endl;

        // if (codec->type == AVMEDIA_TYPE_VIDEO) {
        //     std::cout << "Supported Pixel Formats: ";
        // } else if (codec->type == AVMEDIA_TYPE_AUDIO) {
        //     std::cout << "Supported Sample Formats: ";
        // }

        // 打印支持的比特率、帧率等（如果适用）
        if (codec->supported_framerates) {
            std::cout << "Supported Frame Rates: ";
            for (int i = 0; codec->supported_framerates[i].den != 0; ++i) {
                std::cout << codec->supported_framerates[i].num << "/" << codec->supported_framerates[i].den << " ";
            }
            std::cout << std::endl;
        }

        if (codec->supported_samplerates) {
            std::cout << "Supported Sample Rates: ";
            for (int i = 0; codec->supported_samplerates[i] != 0; ++i) {
                std::cout << codec->supported_samplerates[i] << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cerr << "Codec with ID " << codec_id << " not found." << std::endl;
    }
}