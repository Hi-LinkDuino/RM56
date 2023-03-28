#include "lv_port_disp.h"
#include "lcdc.h"
#include "hal_lcdc.h"
#include "hal_dsi.h"
#include "hal_cache.h"
#include "hal_trace.h"
#include "plat_addr_map.h"
#include "cmsis_os2.h"
#ifdef A7_DISPLAY
#include "best2003.h"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef PSRAMUHS_SIZE
#define PSRAMUHS_SIZE 0x800000
#endif
#define DISPLAY_BUFSIZE (DISPLAY_XRES * DISPLAY_YRES * 4)
#define BUF_NUM 2

enum BUF_STATE {
    IDLE,
    READY,
    BUSY,
};

struct DisplayDevice {
    uint32_t buffers[BUF_NUM];
    volatile enum BUF_STATE buf_state;
    enum DSI_MODE_T mode;
    osSemaphoreId_t sem;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_flush_cb(int error);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
#ifdef A7_DISPLAY
static uint32_t g_buffer0[DISPLAY_BUFSIZE / 4];
static uint32_t g_buffer1[DISPLAY_BUFSIZE / 4];
#endif
static struct DisplayDevice priv = {
    .buffers = {
#ifdef A7_DISPLAY
        g_buffer0,
        g_buffer1,
#else
        PSRAMUHS_BASE + PSRAMUHS_SIZE - DISPLAY_BUFSIZE,
        PSRAMUHS_BASE + PSRAMUHS_SIZE - DISPLAY_BUFSIZE * 2,
#endif
    },
    .buf_state = IDLE,
    .mode = DISPLAY_MODE,
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

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
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    priv.sem = osSemaphoreNew(1, 0, NULL);
    if (priv.sem == NULL) {
        printf("%s: osSemaphoreNew failed\r\n", __func__);
        return;
    }
    for (int i = 0; i < ARRAY_SIZE(priv.buffers); i++) {
        memset((void *)priv.buffers[i], 0, DISPLAY_BUFSIZE);
        printf("buffer[%d] 0x%x\r\n", i, priv.buffers[i]);
    }
    display_init();
    lcdc_initialize(priv.buffers[0], 0);
    lcdc_irq_callback_register(disp_flush_cb);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#ifdef A7_DISPLAY
    L1C_CleanInvalidateCacheRange(1, (size_t)color_p, (size_t)color_p + DISPLAY_BUFSIZE);
#else
    hal_cache_sync(HAL_CACHE_ID_D_CACHE, (uint32_t)color_p, DISPLAY_BUFSIZE);
#endif
    hal_lcdc_lstartaddr(0, (uint32_t)color_p);
    priv.buf_state = READY;
    hal_lcdcl_enable(0, true);
    lcdc_update();
}

static void disp_flush_cb(int error)
{
    priv.buf_state = IDLE;
    lv_disp_flush_ready(&disp_drv);
    osSemaphoreRelease(priv.sem);
}

void wait_for_vsync()
{
    if (priv.buf_state == IDLE) {
        osDelay(16);
    } else {
        osStatus_t ret = osSemaphoreAcquire(priv.sem, 30);
        if (ret) {
            printf("%s error %d\n", __func__, ret);
        }
    }
}

__attribute__((weak)) struct lcd_dev_s *board_lcd_getdev(int lcddev)
{
    return NULL;
}
