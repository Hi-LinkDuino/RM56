/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
//#include "touch_if.h"
//#include "dock/pointer_input_device.h"
//#include "gfx_utils/graphic_log.h"
//#include "graphic_config.h"
#if 1//def CONFIG_DISPLAY_RM69330
/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
static  int16_t HORIZONTAL_RESOLUTION = 480;
/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
static  int16_t VERTICAL_RESOLUTION = 480;
#elif defined(CONFIG_DISPLAY_ICNA3311)
static constexpr int16_t HORIZONTAL_RESOLUTION = 280;
static constexpr int16_t VERTICAL_RESOLUTION = 456;
#elif defined(CONFIG_DISPLAY_A064)
static constexpr int16_t HORIZONTAL_RESOLUTION = 480;
static constexpr int16_t VERTICAL_RESOLUTION = 480;
#endif
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/* event */
#define TOUCH_EVENT_UP (0x01)
#define TOUCH_EVENT_DOWN (0x02)
#define TOUCH_EVENT_MOVE (0x03)
#define TOUCH_EVENT_EXIST (0x04)
#define TOUCH_EVENT_NONE (0x80)

#define TOUCH_DEV_MAX 1
#define TOUCH_MSG_MAX 8

struct touch_msg {
    uint16_t x;
    uint16_t y;
    uint8_t event;
};
static void touchpad_init(void);
static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

static void mouse_init(void);
static bool mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static bool keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static bool encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static bool button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /* Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */
 //     printf("___>>>>>> %s %d\r\n", __FILE__, __LINE__);
     touchpad_init();
	lv_indev_drv_t indev_drv;	 

	/*Register a touchpad input device*/
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = touchpad_read;
	lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/



/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
int instance_handle;
struct touch_msg msg = {0};
static void touchpad_init(void)
{
	#if 0
    /*Your code comes here*/
  //     printf("___>>>>>> %s %d\r\n", __FILE__, __LINE__);
    instance_handle = TouchOpen(0);
        if (!instance_handle) {
            printf("TouchOpen failed");
         //   return nullptr;
        }
  #endif
}

extern uint16_t touch_ad_x,touch_ad_y;
#if 1
bool IsValidTouchMsg(struct touch_msg *msg)
{
    if (msg->x >= HORIZONTAL_RESOLUTION || msg->y >= VERTICAL_RESOLUTION)
        return false;

    if (msg->event != TOUCH_EVENT_UP && msg->event != TOUCH_EVENT_DOWN && msg->event != TOUCH_EVENT_MOVE)
        return false;

    return true;
}

static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	#if 1
    // merge msg with the same event
   //  printf("___>>>>>> %s %d\r\n", __FILE__, __LINE__);
    struct touch_msg tmp[TOUCH_MSG_MAX] = {0};
    int i = 0;
    while (i < TOUCH_MSG_MAX && TouchRead(instance_handle, &tmp[i], 0) == 0) {
        if (!IsValidTouchMsg(&tmp[i]))
            break;

        if (tmp[i].event == TOUCH_EVENT_MOVE) {
            tmp[i].event = TOUCH_EVENT_DOWN;
        }
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
  //      data.point.x = this->msg.x;
  //      data.point.y = this->msg.y;
  //      data.state = (this->msg.event == TOUCH_EVENT_DOWN) ? STATE_PRESS : STATE_RELEASE;
		data->point.x = msg.x;
		data->point.y = msg.y;
		data->state = (msg.event == TOUCH_EVENT_DOWN) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
		
    } else if (i <= TOUCH_MSG_MAX) {
 //       data.point.x = tmp[i - 2].x;
  //      data.point.y = tmp[i - 2].y;
 //       data.state = (tmp[i - 2].event == TOUCH_EVENT_DOWN) ? LV_INDEV_STATE_REL;
		data->point.x =  tmp[i - 2].x;
		data->point.y =tmp[i - 2].y;
		data->state = (msg.event == TOUCH_EVENT_DOWN) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    }
 //   printf("Touch {%d, %d} %d", data->point.x, data->point.y, data->state);
    if (i >= 1 && i <= TOUCH_MSG_MAX && IsValidTouchMsg(&tmp[i - 1]))
        msg = tmp[i - 1];

    return false;
    #else
      struct touch_msg msg;
   		memset(&msg, 0, sizeof(msg));
       
        if (TouchRead(instance_handle, &msg, 100) == 0) {
        			data->point.x = msg.x;
							data->point.y = msg.y;
							data->state = (msg.event == TOUCH_EVENT_DOWN) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
            printf("%s, x %d, y %d", data->state, msg.x, msg.y);
        }
    return false;
    #endif
}
#else
/* Will be called by the library to read the touchpad */
static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	int ret;
	static uint16_t last_x = 0;
	static uint16_t last_y = 0;
	
	ret = touch_scan();
	if(ret != 0)
	{
		last_x = touch_coordinate_x_get(touch_ad_x);
		last_y = touch_coordinate_y_get(touch_ad_y);
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_PR;
	}else{
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_REL;
	}
	return false;

}
#endif

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
	
    return false;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}



#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
