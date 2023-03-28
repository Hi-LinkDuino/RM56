#ifdef CHIP_HAS_RGB_LED
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "app_led.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "cmsis_os2.h"
#include "app_thread.h"
#include "hal_trace.h"

// #include "nvrecord_extension.h"
#if 0//def HITEST
#define LED_TRACE(s,...) 
#else
#define LED_TRACE(s,...)  TRACE(s, ##__VA_ARGS__)
#endif


uint8_t nv_record_get_led_brightness(void);
void nv_record_set_led_brightness(uint8_t b);

#define BRIGHTNESS_DEDAULT 10
static uint8_t brightness = BRIGHTNESS_DEDAULT;
static bool app_led_inited = false;

typedef void (*VoidFunc)(void);
typedef struct _LedConfig {
  enum HAL_PWM_ID_T id;
  VoidFunc init_func;
  bool  hw_blink;
}t_LedConfig;

static const t_LedConfig gaLedConfig[] = {
  {HAL_PWM_ID_1,   hal_iomux_set_pwm1, true},
  {HAL_PWM_ID_2,   hal_iomux_set_pwm2, true},
  {HAL_PWM_ID_3,   hal_iomux_set_pwm3, true},
};

#define INFINITE ((uint16_t)0xffff)

#define LED_NAME(i)   (i==LED_R?"R":i==LED_G?"G":i==LED_B?"B":"N")

typedef struct {
  osTimerId_t   timer;
  bool  onoff; //true-on, false-off
  bool  blinking; // true, auto blinking
  uint16_t off_duration;
  uint16_t on_duration;
  uint16_t counter;  //0xFFFF-infinite, >=0, times of on/off
#ifndef LED_SEPARATELY_CONTROLLED_BY_USER
  uint8_t color;
  bool hw_blink; //MUST be mono color && hw_blink supported
  uint8_t hw_blink_id;
#endif
}t_LedInfo;


#ifdef LED_SEPARATELY_CONTROLLED_BY_USER

static t_LedInfo leds[LED_N];

#else

static t_LedInfo gvLED;

#endif


#define ledConfig(led) (led<LED_N?(&gaLedConfig[led]):NULL)
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
#define ledInfo(led) (led<LED_N?(&leds[led]):NULL)
#else
#define ledInfo(led) (&gvLED)
#endif


static bool all_support_hw_blink(uint32_t led){
  for(int i = 0; i < LED_N; i++){
    if(led & (1<<i)){
      const t_LedConfig *conf = ledConfig(i);
      if(!conf->hw_blink)return false;
    }
  }
  LED_TRACE(0, "%s: led=0x%x", __func__, led);
  return true;
  // return gaLedConfig[LED_R].hw_blink && gaLedConfig[LED_G].hw_blink && gaLedConfig[LED_B].hw_blink
}

static int __led_ctrl_on(enum LED_T led) {
  // if(led >= LED_N || led < 0)return -1;

  struct HAL_PWM_CFG_T pwm_cfg = {
      .freq = 8000, // unit: Hz
      .ratio = brightness,  // duty cycle = high/(high+low)
      .inv = false,
      .sleep_on = false,
  };
  
  const t_LedConfig *conf = ledConfig(led);
  // if(conf->init_func)conf->init_func();
  int ret = hal_pwm_enable(conf->id, &pwm_cfg);
  LED_TRACE(3, "%s(%s) returns %d", __func__, LED_NAME(led), ret);
  return ret;
}

static int __led_ctrl_off(enum LED_T led) {
  // if(led >= LED_N || led < 0)return -1;
  const t_LedConfig *conf = ledConfig(led);
  int ret = hal_pwm_disable(conf->id);
  LED_TRACE(3, "%s(%s) returns %d", __func__, LED_NAME(led), ret);
  return ret;
}


enum LED_MSG_T {
  MSG_LED_ON = 0,
  MSG_LED_OFF,
  MSG_LED_BLINK,
};


static int __led_msg_send(uint32_t msgid, uint32_t p0, uint32_t p1, uint32_t p2) {
    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODULE_LED;
    msg.msg_body.message_id = msgid;
    msg.msg_body.message_ptr = (uint32_t)NULL;
    msg.msg_body.message_Param0 = p0;
    msg.msg_body.message_Param1 = p1;
    msg.msg_body.message_Param2 = p2;
    app_mailbox_put(&msg);
    return 0;
}

#ifndef LED_SEPARATELY_CONTROLLED_BY_USER  
static void __ctrl_bits(uint8_t bits, bool on){
  t_LedInfo *info = &gvLED;
  if(!bits)return;
  if(on){
    for(int i = 0; bits; i++, bits>>=1){
      if(bits&1){
        __led_ctrl_on(i);
        // info->color |= (1<<i);
      }
    }
  }else{
    for(int i = 0; bits; i++, bits>>=1){
      if(bits&1){
        __led_ctrl_off(i);
        // info->color &= ~(1<<i);
      }
    }
  }
}
#endif



static void __led_on(uint32_t led) {
  t_LedInfo *info = ledInfo(led);
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER  
  if(led >= LED_N){
    //turn on all
    for(int i = 0; i < LED_N; i++){
      info = ledInfo(i);
      if(!info->onoff){
        __led_ctrl_on(led);
        info->onoff = true;
      }
    }
  }else if(!info->onoff){
    __led_ctrl_on(led);
    info->onoff = true;
  }
#else
  LED_TRACE(4, "%s: on=%s, %x --> %x", __func__, info->onoff?"true":"false", info->color, led);
  if(!info->onoff || info->color != led){
    if(!info->onoff){
        __ctrl_bits(led, true);
    } else if(info->color != led){
      uint8_t off = info->color & (~led);
      uint8_t on = led & ~(info->color & (~off));
      if(off)__ctrl_bits(off, false);
      if(on)__ctrl_bits(on, true);
    }
    info->color = led;
    info->onoff = true;
  }
#endif
}

static void __led_off(uint32_t led) {
  t_LedInfo *info = ledInfo(led);
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER  
  if(led >= LED_N){
    //turn off all
    for(int i = 0; i < LED_N; i++){
      info = ledInfo(i);
      if(info->onoff){
        __led_ctrl_off(led);
        info->onoff = false;
      }
    }
  }else if(info->onoff){
    __led_ctrl_off(led);
    info->onoff = false;
  }
#else
  // turn off all
  if(info->onoff){
    __ctrl_bits(info->color, false);
    info->onoff = false;
  }
#endif
}

//stop blink，led status not changed
static void __stop_blink(uint32_t led){
  t_LedInfo * info = ledInfo(led);
  if(!info)return;
  if(info->blinking){
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
    const t_LedConfig *conf = ledConfig(led);
    bool hw_blink = conf->hw_blink;
    if(hw_blink){
      hal_pwm_breathing_led_disable(conf->id);
      info->onoff = false; // force off
    }
#else
    bool hw_blink = info->hw_blink;
    if(hw_blink){
      led = info->hw_blink_id;
      for(int i = 0; i < LED_N; i++){
        if(led & (1<<i)){
          const t_LedConfig *conf = ledConfig(i);
          hal_pwm_breathing_led_disable(conf->id);
        }
      }
      info->onoff = false; // force off
      info->hw_blink = false;
    }
#endif

    LED_TRACE(0, "%s, hw_blink=%s, counter=%x", __func__, hw_blink?"true":"false", info->counter);
    if(!hw_blink || info->counter != INFINITE){
      // sw blink or hw blink & not INFINITE, stop timer
      osTimerStop(info->timer);
    }
    info->blinking = false;
    LED_TRACE(2, "%s: LED(%s) blink stopped", __func__, LED_NAME(led));
  }
}
static void _led_timer_handler(void *param){
  int i = (int)param;
  t_LedInfo * info = ledInfo(i);
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
  if(i < 0 || i >= LED_N){
    LED_TRACE(2, "%s: out of range(%d)", __func__, i);
    return;
  }
  const t_LedConfig *conf = ledConfig(i);
  if(conf->hw_blink)
#else
  if(info->hw_blink)
#endif
  {
    //hw blink, time expiration means to stop blink
    __stop_blink(i);
    return;
  }
  LED_TRACE(0, "%s, led=0x%x, on=%s", __func__, info->color, info->onoff?"true":"false");
  if(info->onoff){
    __led_off(i);
    if(info->counter != INFINITE){
      info->counter--;
      if(info->counter == 0){
        //blink counter reaches 0, stop blinking
        __stop_blink(i);
        return;
      }
    }
    osTimerStart(info->timer, info->off_duration);
  }else{
    LED_TRACE(3, "%s: blink counter = 0x%x, duration=%d,%dms", __func__, info->counter, info->on_duration, info->off_duration);
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
    __led_on(i);
#else
    __led_on(info->color);
#endif
    osTimerStart(info->timer, info->on_duration);
  }

}


static void __hw_blink(enum LED_T i, t_LedInfo *info, const t_LedConfig *conf){
  LED_TRACE(0, "%s: %d, led=%d", __func__, conf->hw_blink, i);
  if(!conf->hw_blink)return;
  struct HAL_PWM_BR_CFG_T br = {
      .off_time_ms = info->off_duration,
      .on_time_ms = info->on_duration,
      .fade_time_ms = 0,
  };
  hal_pwm_breathing_led_enable(conf->id, &br);
  if (info->counter != INFINITE) {
    if (!info->timer)
      info->timer = osTimerNew(_led_timer_handler, osTimerOnce, (void *)i, NULL);
    osTimerStart(info->timer, (uint32_t)info->counter * (info->on_duration + info->off_duration));
  }
#ifndef LED_SEPARATELY_CONTROLLED_BY_USER
  info->hw_blink = true;
  info->hw_blink_id = i;
#endif
}
#ifndef LED_SEPARATELY_CONTROLLED_BY_USER
static void __hw_blink_all(uint32_t led, t_LedInfo *info){
  LED_TRACE(0, "%s: led=0x%x", __func__, led);

  struct HAL_PWM_BR_CFG_T br = {
      .off_time_ms = info->off_duration,
      .on_time_ms = info->on_duration,
      .fade_time_ms = 0,
  };
  for(int i = 0; i < LED_N; i++){
    if(led & (1<<i)){
      const t_LedConfig *conf = ledConfig(i);
      ASSERT(conf->hw_blink, "led(%d) not support hw_blink", i);
      hal_pwm_breathing_led_enable(conf->id, &br);
    }
  }
  if (info->counter != INFINITE) {
    if (!info->timer)
      info->timer = osTimerNew(_led_timer_handler, osTimerOnce, (void *)led, NULL);
    osTimerStart(info->timer, (uint32_t)info->counter * (info->on_duration + info->off_duration));
  }
  info->hw_blink = true;
  info->hw_blink_id = led;
}
#endif

static void __led_blink(uint32_t led, uint16_t on_duration_ms, uint16_t off_duration_ms, uint16_t counter) {
  if(counter == 0)counter = INFINITE;
  if(off_duration_ms == 0)off_duration_ms = on_duration_ms;

  LED_TRACE(0, "%s LED(%02x), on=%d,off=%d,counter=0x%x", __func__, led, on_duration_ms, off_duration_ms, counter);
  t_LedInfo *info = ledInfo(led);
  info->blinking = true;
  info->on_duration = on_duration_ms;
  info->off_duration = off_duration_ms;
  info->counter = counter;

  __led_off(led);
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
  const t_LedConfig *conf = ledConfig(led);
  if(conf->hw_blink){
    //hw blink
    __hw_blink(led, info, conf);
    return;
  }
#else
  info->color = led;
  if(all_support_hw_blink(led)){
    __hw_blink_all(led, info);
    return;    
  }
#endif
  // MUST use sw blink
  __led_on(led);
  if (!info->timer)
    info->timer = osTimerNew(_led_timer_handler, osTimerOnce, (void*)led, NULL);
#ifndef LED_SEPARATELY_CONTROLLED_BY_USER
  info->hw_blink = false;
#endif
  osTimerStart(info->timer, on_duration_ms);    
}


static int app_led_handle_process(APP_MESSAGE_BODY *msg_body)
{
  uint32_t i = msg_body->message_Param0;
  uint16_t on_ms = msg_body->message_Param1 >> 16;
  uint16_t off_ms = msg_body->message_Param1 & 0xffff;

  LED_TRACE(0, "%s msgid=%d, led=0x%x", __func__, msg_body->message_id, i);
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
  if (msg_body->message_id == MSG_LED_BLINK && i >= LED_N)
    return 0;
#else
  i &= LED_MASK;
  if (msg_body->message_id != MSG_LED_OFF && i == 0)
    return 0;
#endif

  switch (msg_body->message_id) {
    case MSG_LED_ON:
      __stop_blink(i);
      __led_on(i);
      break;
    case MSG_LED_OFF:
      __stop_blink(i);
      __led_off(i);
#ifndef LED_SEPARATELY_CONTROLLED_BY_USER  
      gvLED.color = 0;
#endif
      break;
    case MSG_LED_BLINK:
      __stop_blink(i);
      __led_blink(i, on_ms, off_ms, msg_body->message_Param2);
      break;
  }

  return 0;
}

void app_led_init(void){
  if(!app_led_inited){
    app_set_threadhandle(APP_MODULE_LED, app_led_handle_process);
    
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
    memset(leds, 0, sizeof(leds));
#else
    memset(&gvLED, 0, sizeof(gvLED));
#endif
    for(int i = 0; i < LED_N; i++){
      if(gaLedConfig[i].init_func)gaLedConfig[i].init_func();
    }

    brightness = nv_record_get_led_brightness();
    if(brightness == 0 || brightness == 0xff)brightness = BRIGHTNESS_DEDAULT;

    app_led_inited = true;
  }
}



#ifdef LED_SEPARATELY_CONTROLLED_BY_USER

void led_blink(enum LED_T led, uint16_t on_duration_ms, uint16_t off_duration_ms, uint16_t counter){
  if(led >= LED_N || led < 0)return;
  __led_msg_send(MSG_LED_BLINK, led, (on_duration_ms<<16)|off_duration_ms, counter);
}

void led_on(enum LED_T led) {
  if(led >= LED_N || led < 0)return;
  __led_msg_send(MSG_LED_ON, led, 0, 0);
}

void led_off(enum LED_T led) {
  if(led >= LED_N || led < 0)return;
  __led_msg_send(MSG_LED_OFF, led, 0, 0);
}
#else

void led_blink(uint8_t bits, uint16_t on_duration_ms, uint16_t off_duration_ms, uint16_t counter){
  __led_msg_send(MSG_LED_BLINK, bits, (on_duration_ms<<16)|off_duration_ms, counter);
}

void led_on(uint8_t bits) {
  __led_msg_send(MSG_LED_ON, bits, 0, 0);
}

void led_off(void) {
  __led_msg_send(MSG_LED_OFF, 0, 0, 0);
}

#endif
void led_set_brightness(uint8_t b){
  if(brightness != b && b > 0 && b <= 100){
    nv_record_set_led_brightness(b);
    brightness = b;
  }
}
uint8_t led_get_brightness(void){
  return brightness;
}

#endif