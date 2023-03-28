#include "hal_dma.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include <string.h>

#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))
#define BUFSIZE 0x10000
static char src[BUFSIZE];
static char dst[BUFSIZE];

int dma_memcpy(void *dst, const void *src, int len)
{
    struct HAL_DMA_CH_CFG_T dma_cfg = {0};
    int remains = len;
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
    dma_cfg.dst_periph = HAL_GPDMA_MEM;
    dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    dma_cfg.handler = NULL;
    dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
    dma_cfg.src_periph = HAL_GPDMA_MEM;
    dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    dma_cfg.type = HAL_DMA_FLOW_M2M_DMA;
    dma_cfg.try_burst = false;
    dma_cfg.ch = hal_dma_get_chan(dma_cfg.dst_periph, HAL_DMA_LOW_PRIO);
    if (dma_cfg.ch == HAL_DMA_CHAN_NONE) {
        printf("hal_dma_get_chan failed\r\n");
        return -1;
    }

    dma_cfg.dst = (uint32_t)dst;
    dma_cfg.src = (uint32_t)src;

    while (remains > 0) {
        if (remains > HAL_DMA_MAX_DESC_XFER_SIZE) {
            dma_cfg.src_tsize = HAL_DMA_MAX_DESC_XFER_SIZE;
        } else {
            dma_cfg.src_tsize = remains;
        }
        if (hal_dma_start(&dma_cfg) != HAL_DMA_OK) {
            printf("hal_dma_start failed\r\n");
            hal_dma_free_chan(dma_cfg.ch);
            return len - remains;
        }

        while (hal_dma_chan_busy(dma_cfg.ch))
            ;

        dma_cfg.dst += dma_cfg.src_tsize * 4;
        dma_cfg.src += dma_cfg.src_tsize * 4;
        remains -= dma_cfg.src_tsize;
    }
    hal_dma_free_chan(dma_cfg.ch);
    return len;
}

void dma_test(void)
{
    uint32_t now = GET_CURRENT_MS();
    memcpy(dst, src, BUFSIZE);
    printf("memcpy used time %u ms\r\n", GET_CURRENT_MS() - now);
    for (int i = 0; i < BUFSIZE; i++) {
        src[i] = i;
    }
    now = GET_CURRENT_MS();
    int len = dma_memcpy(dst, src, BUFSIZE / 4); // dma use word length
    printf("dma_memcpy used time %u ms\r\n", GET_CURRENT_MS() - now);
    if (len != BUFSIZE / 4 || memcmp(dst, src, BUFSIZE) != 0)
        printf("dma_memcpy failed\r\n");
}
