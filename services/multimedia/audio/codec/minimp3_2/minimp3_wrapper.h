#ifndef __MINIMP3_WRAPPER_H__
#define __MINIMP3_WRAPPER_H__

#include "minimp3.h"
#include "minimp3_ex.h"

typedef mp3dec_t minimp3_wrapper_mp3dec_t;
typedef mp3d_sample_t minimp3_wrapper_mp3d_sample_t;
typedef mp3dec_frame_info_t minimp3_wrapper_mp3dec_frame_info_t;
typedef mp3dec_io_t minimp3_wrapper_mp3dec_io_t;
typedef MP3D_ITERATE_CB MINIMP3_WRAPPER_MP3D_ITERATE_CB;

#ifdef __cplusplus
extern "C"
{
#endif

    void minimp3_wrapper_mp3dec_init(minimp3_wrapper_mp3dec_t *dec);

    int minimp3_wrapper_mp3dec_decode_frame(minimp3_wrapper_mp3dec_t *dec, const uint8_t *mp3, int mp3_bytes, minimp3_wrapper_mp3d_sample_t *pcm, minimp3_wrapper_mp3dec_frame_info_t *info);

    int minimp3_wrapper_mp3dec_detect_buf(const uint8_t *buf, size_t buf_size);

    int minimp3_wrapper_mp3dec_detect_cb(minimp3_wrapper_mp3dec_io_t *io, uint8_t *buf, size_t buf_size);

    int minimp3_wrapper_mp3dec_iterate_buf(const uint8_t *buf, size_t buf_size, MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *user_data);

    int minimp3_wrapper_mp3dec_iterate_cb(minimp3_wrapper_mp3dec_io_t *io, uint8_t *buf, size_t buf_size, MINIMP3_WRAPPER_MP3D_ITERATE_CB callback, void *user_data);

#ifdef __cplusplus
}
#endif

#endif