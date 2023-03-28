#include "hal_trace.h"
#include "vow_alsa.h"
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "alsa.h"
#include "alsa_extra.h"

typedef struct {
    alsa_handle_t *alsa;
} sink_alsa_ctx;


void sink_alsa_stop(void* handle) {
    if (!handle) return;
    sink_alsa_ctx* ctx = (sink_alsa_ctx*)handle;
    if (ctx->alsa) {
        
    }
}

void sink_alsa_close(void* handle) {
    if (!handle) return;
    sink_alsa_stop(handle);
    sink_alsa_ctx* ctx = (sink_alsa_ctx*)handle;
    if (ctx->alsa) {
        alsa_stop(ctx->alsa);
        alsa_close(ctx->alsa);
        ctx->alsa = NULL;
    }
    free(ctx);
}

void* sink_alsa_open(unsigned int sample_rate, int channel, int format_size) { 
    sink_alsa_ctx* ctx;
    do {
        ctx = (sink_alsa_ctx *)malloc(sizeof(sink_alsa_ctx));
        if (!ctx) {
            TRACE(0, "Err:malloc\n");
            break;
        }
        memset(ctx, 0, sizeof(sink_alsa_ctx));
        
        ctx->alsa = alsa_open(ALSA_MODE_OUT, sample_rate, channel, 0);
        TRACE(0, "alsa_open");
        // alsa_volume_set(100); // 10
        return ctx;
    } while(0);

    sink_alsa_close(ctx);
    return NULL;
}


int sink_alsa_start(void* handle) {
    if (!handle) return -1;
    sink_alsa_ctx* ctx = (sink_alsa_ctx*)handle;
    if (ctx->alsa) {
        alsa_start(ctx->alsa);
    }
    return 0;
}

int sink_alsa_write(void* handle, unsigned char* buffer, int buffer_size) {
    if (!handle) return -1;
    sink_alsa_ctx* ctx = (sink_alsa_ctx*)handle;
    if (ctx->alsa) {
        alsa_write(ctx->alsa, buffer, buffer_size);
    }
    return 0;
}

int sink_alsa_volume_get() {
    float rate;
    int ret = (uint32_t)alsa_volume_float_get(&rate);
    if (ret == 0) {
        return (int)rate;
    } else {
        return -1;
    }
}

void sink_alsa_volume_set(int vol) {
    if(vol > 100) {
        vol = 100;
    }
    
    float set_val = vol / 100.0f;
    TRACE(0,"vol:%d", vol);
    alsa_volume_float_set(set_val);
}

#if 0
sink_op_t sink_alsa_op = {
    .open = sink_alsa_open,
    .close = sink_alsa_close,
    .write = sink_alsa_write,
    .start = sink_alsa_start,
    .stop = sink_alsa_stop,
    .pause = NULL,
    .resume = NULL,
    .volume_get = sink_alsa_volume_get,
    .volume_set = sink_alsa_volume_set
};
#endif