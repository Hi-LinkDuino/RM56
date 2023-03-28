#ifndef LPLAYER_STREAM_INTERFACE_H
#define LPLAYER_STREAM_INTERFACE_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lplayer.h"

#define SOURCE_BUFFER_MAX_SIZE      100 * 1024
#define SOURCE_BUFFER_MIN_SIZE      30  * 1024

typedef struct
{
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits;
} stream_player_format_t;

void *audio_memory_buffer_create(uint32_t buffer_size);

void audio_memory_buffer_close(void *handle);

int audio_memory_buffer_write(void *handle, uint8_t *buffer, uint32_t len, bool eos_flag);

void audio_memory_buffer_reset(void *handle);

#endif