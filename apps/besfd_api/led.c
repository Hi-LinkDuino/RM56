
#ifdef PRJ_SWIFT

#include "besfd_api.h"
#include "app_led.h"
#include "hal_trace.h"

#define SWIFT_S_MIC_MUTE        LED(R)
#define SWIFT_S_WIFI_CONNECTED  LED(G)
#define SWIFT_S_BT_CONNECTED    LED(B)


static uint8_t current_swift_led_scene = 0;
static void __swift_led_scene_set(uint8_t scene){
  TRACE(0, "%s %02x---->%02x", __func__, current_swift_led_scene, scene);
  if(scene == current_swift_led_scene)return;
  current_swift_led_scene = scene;
  if(scene){
    led_blink(scene, 500, 200, 0);
  } else {
    led_off();
  }
}

void swift_led_scene_update(void){
  uint8_t scene = 0;
  if(swift_mic_is_mute())scene |= SWIFT_S_MIC_MUTE;
  if(bt_hfp_connected() || bt_a2dp_connected()) scene |= SWIFT_S_BT_CONNECTED;
  if(wifi_sta_connected())scene |= SWIFT_S_WIFI_CONNECTED;

  __swift_led_scene_set(scene);
}

#endif
