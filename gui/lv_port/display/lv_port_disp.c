#include "lv_port_disp.h"
#include "plat_addr_map.h"
#include "hal_dsi.h"
#include "hal_cache.h"
#include "hal_trace.h"
#include "cmsis_os2.h"
#include "cmsis.h"

/*********************
 *      DEFINES
 *********************/
#ifndef PSRAMUHS_SIZE
#define PSRAMUHS_SIZE 0x800000
#endif
#define DISPLAY_BUFSIZE ((DISPLAY_XRES * DISPLAY_YRES * 4 + 0xfffU) & (~0xfffU))
#define BUF_NUM 2

enum BUF_STATE {
    IDLE,
    READY,
    BUSY,
};

struct MipiDsiDevice {
    uint32_t buffers[BUF_NUM];
    volatile uint8_t buf_idx;
    volatile uint8_t free_chan;
    volatile enum BUF_STATE buf_state;
    enum DSI_MODE_T mode;
    struct HAL_DSI_CFG_T *cfg;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_flush_cb(uint8_t layerId, uint8_t channel, uint32_t addr);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
#ifdef FB_IN_PSRAM
static uint32_t buffer0[DISPLAY_BUFSIZE / 4];
static uint32_t buffer1[DISPLAY_BUFSIZE / 4];
#endif
static struct MipiDsiDevice priv = {
    .buffers = {
#ifdef FB_IN_PSRAM
        (uint32_t)buffer0,
        (uint32_t)buffer1,
#else
        PSRAMUHS_BASE + PSRAMUHS_SIZE - DISPLAY_BUFSIZE,
        PSRAMUHS_BASE + PSRAMUHS_SIZE - DISPLAY_BUFSIZE * 2,
#endif
    },
    .buf_idx = 0,
    .free_chan = 0,
    .buf_state = IDLE,
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
     printf("Welcome to lv_port_disp_init1\r\n");
    disp_init();
	printf("Welcome to lv_port_disp_init2\r\n");

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc, (void *)priv.buffers[0], (void *)priv.buffers[1], DISPLAY_XRES * DISPLAY_YRES);

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = DISPLAY_XRES;
    disp_drv.ver_res = DISPLAY_YRES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;

    /*Required for Example 3)*/
    disp_drv.full_refresh = 1;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
	printf("Welcome to lv_port_disp_init3\r\n");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    for (int i = 0; i < ARRAY_SIZE(priv.buffers); i++) {
        memset((void *)priv.buffers[i], 0, DISPLAY_BUFSIZE);
        hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)priv.buffers[i], DISPLAY_BUFSIZE);
        printf("buffer[%d] 0x%x\r\n", i, priv.buffers[i]);
    }
    display_get_config(&priv.cfg, &priv.mode);
    display_init();
    hal_lcdc_init(priv.cfg, NULL, (const uint8_t *)priv.buffers[0], NULL);
    hal_dsi_start();
    hal_lcdc_set_callback(disp_flush_cb);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    hal_cache_sync(HAL_CACHE_ID_D_CACHE, color_p, DISPLAY_BUFSIZE);
    uint32_t irqflags = int_lock();
    if (priv.mode == DSI_MODE_VIDEO) {
        hal_lcdc_update_addr(1, priv.free_chan, color_p);
    } else {
        hal_lcdc_update_addr(1, priv.buf_idx, color_p);
        hal_lcdc_start(); // trigger transmission, used in DSI_CMD_MODE
    }
    priv.buf_state = READY;
    priv.buf_idx = (priv.buf_idx + 1) % BUF_NUM;
    int_unlock(irqflags);
}

static void disp_flush_cb(uint8_t layerId, uint8_t channel, uint32_t addr)
{
    uint8_t ready_buf_idx;
    priv.free_chan = channel;
    switch (priv.buf_state) {
    case READY:
        // free channel = next channel
        if (priv.buf_idx >= 1) {
            ready_buf_idx = priv.buf_idx - 1;
        } else {
            ready_buf_idx = BUF_NUM - 1;
        }
        hal_lcdc_update_addr(1, channel, priv.buffers[ready_buf_idx]);
        priv.buf_state = BUSY;
        if (priv.mode == DSI_MODE_VIDEO) {
            break;
        }
        // else fallthrough
    case BUSY:
        priv.buf_state = IDLE;
        /*IMPORTANT!!!
         *Inform the graphics library that you are ready with the flushing*/
        lv_disp_flush_ready(&disp_drv);
        break;
    default:
        break;
    }
}

void wait_for_vsync()
{
    if (priv.buf_state == IDLE) {
        osDelay(16);
        return;
    }
    uint32_t cnt = 0;
    while (priv.buf_state == READY) {
        if (cnt++ > 100) {
            printf("READY -> BUSY error, chan %d, cur_buf %d\n", priv.free_chan, priv.buf_idx);
            break;
        }
        osDelay(1);
    }
}

uint32_t display_calc_pixel_clk(uint16_t hpixel, uint16_t vline, uint8_t frameRate)
{
    uint32_t pixNum = hpixel * vline * frameRate;
    if ((pixNum % 1000) == 0) {
        return pixNum / 1000;
    }
    return (pixNum / 1000 + 1);
}

uint32_t display_calc_data_rate(uint16_t hpixel, uint16_t vline, uint8_t frameRate, uint8_t bpp, uint8_t lane_number)
{
    uint32_t bitClk;
    uint32_t bitNum = hpixel * vline * frameRate * bpp;
    if ((bitNum % 1000000) == 0) {
        bitClk = bitNum / 1000000;
    } else {
        bitClk = bitNum / 1000000 + 1;
    }
    if (lane_number == 0) {
        lane_number = 1; // default 1 lane
    }
    if ((bitClk % lane_number) == 0) {
        return bitClk / lane_number;
    }
    return bitClk / lane_number + 1;
}

void display_read_id()
{
    uint8_t read_id[3] = {0};
    hal_dsi_read_cmd(0x04, read_id, 3);
    printf("read id %02X-%02X-%02X\n", read_id[0], read_id[1], read_id[2]);
}
