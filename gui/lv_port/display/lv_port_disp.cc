/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "plat_addr_map.h"
#include "disp_hal.h"
#include "hal_dsi.h"
#include "hal_cache.h"
#include "cmsis_os2.h"
#include "cmsis.h"
//#include "fbdev.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
 #define LV_HOR_RES_MAX 480
 #define LV_VER_RES_MAX 480
#define LV_BUF_SIZE (LV_HOR_RES_MAX*LV_VER_RES_MAX)		// 800x480

static lv_disp_draw_buf_t disp_buf_1;
// static lv_color_t buf1_1[LV_BUF_SIZE];						/*A buffer for 10 rows*/
// static lv_color_t buf1_2[LV_BUF_SIZE];						/*A buffer for 10 rows*/
static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/

static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_flush_cb(uint8_t layerId, uint8_t channel, uint32_t addr);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
        const lv_area_t * fill_area, lv_color_t color);
#endif
// static char *fbp = 0;
// static struct DispInfo info = {0};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define BUF_NUM 2
#define DISPLAY_BUFSIZE ((LV_HOR_RES_MAX * LV_VER_RES_MAX * 4 + 0xfffU) & (~0xfffU))
#ifndef PSRAMUHS_SIZE
#define PSRAMUHS_SIZE 0x800000
#endif

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
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LVGL requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows: 
     *      LVGL will draw the display's content here and writes it to your display
     * 
     * 2. Create TWO buffer with some rows: 
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     * 
     * 3. Create TWO screen-sized buffer: 
     *      Similar to 2) but the buffer have to be screen sized. When LVGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

    /* Example for 3) */
    lv_disp_draw_buf_init(&disp_buf_1, (void *)priv.buffers[0], (void *)priv.buffers[1], LV_BUF_SIZE);

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &disp_buf_1;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
    printf("___>>>>>> %s %d\r\n", __FILE__, __LINE__);
    DispInit(0);
    DispOn(0);
    hal_lcdc_set_callback(disp_flush_cb);
}

int __doPanelRst(void)
{
    /*You code here*/
   DispOn(0);
   hal_lcdc_set_callback(disp_flush_cb);
   return 0;
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


/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for(i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color)
{
    /*It's an example code which should be done by your GPU*/
    int32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y <= fill_area->y2; y++) {
        for(x = fill_area->x1; x <= fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}

#endif  /*LV_USE_GPU*/

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
