#include "lv_port_indev.h"
#include "touch_fts.h"
#include "hal_trace.h"

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t *indev_touchpad;
static DevHandle g_handle;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static int TouchRead(DevHandle handle, struct touch_msg *msg, uint32_t timeout);
static DevHandle _TouchOpen(int id);
static void TouchClose(DevHandle handle);

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    g_handle = _TouchOpen(0);
}

bool IsValidTouchMsg(struct touch_msg *msg)
{
    if (msg->x >= TPD_RES_X || msg->y >= TPD_RES_Y)
        return false;

    if (msg->event != TOUCH_EVENT_UP && msg->event != TOUCH_EVENT_DOWN)
        return false;

    return true;
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static struct touch_msg msg = {0};
    // merge msg with the same event
    struct touch_msg tmp[TOUCH_MSG_MAX] = {0};
    int i = 0;
    while (i < TOUCH_MSG_MAX && TouchRead(g_handle, &tmp[i], 0) >= 0) {
        if (!IsValidTouchMsg(&tmp[i]))
            break;

        i++;
        if (IsValidTouchMsg(&msg)) {
            if (tmp[i - 1].event != msg.event) {
                break;
            }
        } else {
            if (i > 1 && tmp[i - 1].event != tmp[0].event)
                break;
        }
    }
    if (i <= 1) {
        data->point.x = msg.x;
        data->point.y = msg.y;
        data->state = (msg.event == TOUCH_EVENT_DOWN) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    } else if (i <= TOUCH_MSG_MAX) {
        data->point.x = tmp[i - 2].x;
        data->point.y = tmp[i - 2].y;
        data->state = (tmp[i - 2].event == TOUCH_EVENT_DOWN) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    }
    data->continue_reading = false;
    // printf("%s: data {%d, %d} state %d\r\n", __func__, data->point.x, data->point.y, data->state);
    if (i >= 1 && i <= TOUCH_MSG_MAX && IsValidTouchMsg(&tmp[i - 1]))
        msg = tmp[i - 1];
}

extern struct touch_device g_touch_fts;
extern struct touch_device g_touch_ztw523;
struct TouchManager {
    struct touch_device *device[TOUCH_DEV_MAX];
    uint32_t deviceNum;
};
static struct TouchManager g_touchManager = {
#ifdef CONFIG_DISPLAY_A064
        {&g_touch_fts}, 1,
#elif defined(CONFIG_DISPLAY_RM69330)
        {&g_touch_ztw523}, 1,
#else
		.deviceNum = 0,
#endif
};

static void TouchTask(void *arg)
{
    struct touch_device *dev = (struct touch_device *)arg;
    struct touch_msg last_msg = {0};
    dev->irq_enable(true); // enable irq first
    while (1) {
        if (osSemaphoreAcquire(dev->sem, osWaitForever) != 0) {
            continue;
        }
        dev->irq_enable(false); // avoid too much irq
        struct touch_msg msg;
        if (dev->read(dev, &msg) == 0) {
            if (memcmp(&msg, &last_msg, sizeof(struct touch_msg)) != 0) {
                if (osMessageQueuePut(dev->mq, &msg, 0, osWaitForever) != 0) {
                    printf("failed to enqueue touch_msg\r\n");
                } else {
                    last_msg = msg; // filter repeated msg
                }
            }
        }
        dev->irq_enable(true); // enable irq after data process
    }
}

static int TouchRead(DevHandle handle, struct touch_msg *msg, uint32_t timeout)
{
    struct touch_device *dev = (struct touch_device *)handle;
    if (!dev || !dev->mq || !msg) {
        return -1;
    }
    if (osMessageQueueGetCount(dev->mq) == 0) {
        return -1;
    }
    osMessageQueueGet(dev->mq, msg, NULL, timeout);
    return osMessageQueueGetCount(dev->mq);
}

static DevHandle _TouchOpen(int id)
{
	printf("Welcome to TouchOpen\r\n");

    if (id < 0 || id >= g_touchManager.deviceNum) {
        return NULL;
    }
    struct touch_device *dev = g_touchManager.device[id];
    if (dev->init(dev) != 0) {
        printf("%s: touch device init failed\r\n", __func__);
        return NULL;
    }

    dev->sem = osSemaphoreNew(1, 0, NULL);
    if (dev->sem == NULL) {
        TouchClose(dev);
        printf("%s: osSemaphoreNew failed\r\n", __func__);
        return NULL;
    }
    dev->mq = osMessageQueueNew(TOUCH_MSG_MAX, sizeof(struct touch_msg), NULL);
    if (dev->mq == NULL) {
        TouchClose(dev);
        printf("%s: osSemaphoreNew failed\r\n", __func__);
        return NULL;
    }

    osThreadAttr_t attr = {0};
    attr.stack_size = 2048;
    attr.priority = osPriorityAboveNormal;
    attr.name = dev->name;
    dev->tid = osThreadNew((osThreadFunc_t)TouchTask, dev, &attr);
    if (dev->tid == NULL) {
        TouchClose(dev);
        printf("%s: osThreadNew failed\r\n", __func__);
        return NULL;
    }
    return dev;
}

static void TouchClose(DevHandle handle)
{
    struct touch_device *dev = (struct touch_device *)handle;
    if (!dev) {
        return;
    }
    if (dev->deinit) {
        dev->deinit();
    }
    if (dev->mq) {
        osMessageQueueDelete(dev->mq);
        dev->mq = NULL;
    }
    if (dev->sem) {
        osSemaphoreDelete(dev->sem);
        dev->sem = NULL;
    }
    if (dev->tid) {
        osThreadTerminate(dev->tid);
        dev->tid = NULL;
    }
}
