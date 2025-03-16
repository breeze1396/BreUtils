#pragma once
extern "C" {
#include <libavutil/error.h>
}
#include <string>
std::string GetFFmpegError(int errnum){
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
    return std::string(errbuf);
}
