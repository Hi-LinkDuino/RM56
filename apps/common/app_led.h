#ifndef APP_LED_H
#define APP_LED_H

#include <stdint.h>

enum LED_T {
  LED_R,
  LED_G,
  LED_B,
  LED_N,
};


#ifdef LED_SEPARATELY_CONTROLLED_BY_USER


#define led_all_on()   led_on(LED_N)
#define led_all_off()  led_off(LED_N)
#define led_r_on()   led_on(LED_R)
#define led_r_off()  led_off(LED_R)
#define led_g_on()   led_on(LED_G)
#define led_g_off()  led_off(LED_G)
#define led_b_on()   led_on(LED_B)
#define led_b_off()  led_off(LED_B)

#define led_r_blink(on_ms, off_ms, counter)   led_blink(LED_R, on_ms, off_ms, counter)
#define led_g_blink(on_ms, off_ms, counter)   led_blink(LED_G, on_ms, off_ms, counter)
#define led_b_blink(on_ms, off_ms, counter)   led_blink(LED_B, on_ms, off_ms, counter)

#else

#define LED_MASK  (LED(R)|LED(G)|LED(B))

#define _LED(x) (LED_##x)
#define LED(x)  (1<<_LED(x))

#define LED_W     (LED(R)|LED(G)|LED(B))    //white
#define LED_Y     (LED(R)|LED(G))           //yellow
#define LED_M     (LED(R)|LED(B))           //magenta
#define LED_C     (LED(G)|LED(B))           //cyan


#define led_r_on()   led_on(LED(R))
#define led_g_on()   led_on(LED(G))
#define led_b_on()   led_on(LED(B))
#define led_w_on()   led_on(LED_W)
#define led_y_on()   led_on(LED_Y)
#define led_m_on()   led_on(LED_M)
#define led_c_on()   led_on(LED_C)


#define led_r_blink(on_ms, off_ms, counter)   led_blink(LED(R), on_ms, off_ms, counter)
#define led_g_blink(on_ms, off_ms, counter)   led_blink(LED(G), on_ms, off_ms, counter)
#define led_b_blink(on_ms, off_ms, counter)   led_blink(LED(B), on_ms, off_ms, counter)
#define led_w_blink(on_ms, off_ms, counter)   led_blink(LED_W, on_ms, off_ms, counter)
#define led_y_blink(on_ms, off_ms, counter)   led_blink(LED_Y, on_ms, off_ms, counter)
#define led_m_blink(on_ms, off_ms, counter)   led_blink(LED_M, on_ms, off_ms, counter)
#define led_c_blink(on_ms, off_ms, counter)   led_blink(LED_C, on_ms, off_ms, counter)


#endif


#ifdef __cplusplus
extern "C" {
#endif

void app_led_init(void);

#ifdef LED_SEPARATELY_CONTROLLED_BY_USER

/**
 * @brief control led blinking: on->off->on->...->off
 * 
 * @param led               a single led
 * @param on_duration_ms    leds on time in milliseconds
 * @param off_duration_ms   leds off time in milliseconds, just use on_duration_ms if 0
 * @param counter           repeat times, 0xffff for INFINITE
 */
void led_blink(enum LED_T led, uint16_t on_duration_ms, uint16_t off_duration_ms, uint16_t counter);
void led_on(enum LED_T led);
void led_off(enum LED_T led);

#else

/**
 * @brief control led blinking: on->off->on->...->off
 * 
 * @param bits              led bits, like LED(R)|LED(G) to control R&G
 * @param on_duration_ms    leds on time in milliseconds
 * @param off_duration_ms   leds off time in milliseconds, just use on_duration_ms if 0
 * @param counter           repeat times, 0xffff for INFINITE
 */
void led_blink(uint8_t bits, uint16_t on_duration_ms, uint16_t off_duration_ms, uint16_t counter);
void led_on(uint8_t bits);
void led_off(void);

#endif

void led_set_brightness(uint8_t brightness);
uint8_t led_get_brightness(void);

#ifdef __cplusplus
}
#endif

#endif