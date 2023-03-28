#include "hal_cmu.h"
#include "hal_iomux_best2003.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "cmsis_os2.h"
#include "stdio.h"
#include "hal_trace.h"
enum {
    UNUSED=0,
    DO,
    RE,
    MI,
    FA,
    SO,
    LA,
    XI,
    DO1,
    RI1,
    TONE_MAX,
};

#define ON 0
#define OFF 1

uint16_t tone_freq[TONE_MAX] = {0, 1047, 1175, 1319, 1397, 1568, 1760, 1967, 4186, 4698 };

typedef struct tone_s
{
    int tone;
    int delay_ms;
} tone_t;

#define DEF_DELAY 350
static tone_t tiger_notation[]=
{
    {DO, DEF_DELAY},
    {RE, DEF_DELAY},
    {MI, DEF_DELAY},
    {DO, DEF_DELAY*2},

    {DO, DEF_DELAY},
    {RE, DEF_DELAY},
    {MI, DEF_DELAY},
    {DO, DEF_DELAY*2},

    {MI, DEF_DELAY},
    {FA, DEF_DELAY},
    {SO, DEF_DELAY*2},

    {MI, DEF_DELAY},
    {FA, DEF_DELAY},
    {SO, DEF_DELAY*2},

    {SO, DEF_DELAY},
    {LA, DEF_DELAY},
    {SO, DEF_DELAY},
    {FA, DEF_DELAY},
    {MI, DEF_DELAY},
    {DO, DEF_DELAY*2},

    {SO, DEF_DELAY},
    {LA, DEF_DELAY},
    {SO, DEF_DELAY},
    {FA, DEF_DELAY},
    {MI, DEF_DELAY},
    {DO, DEF_DELAY*2},

    {RE, DEF_DELAY},
    {SO, DEF_DELAY},
    {DO, DEF_DELAY*2},

    {RE, DEF_DELAY},
    {SO, DEF_DELAY},
    {DO, DEF_DELAY*2},
};
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#ifndef  PWM_BEEP
#define  PWM_BEEP HAL_IOMUX_PIN_P1_1
#endif

static void pwm_init(void)
{
    static bool b_pwm_init_flag = false;

    if (!b_pwm_init_flag) {
        static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_pwm[] = {
                {PWM_BEEP, HAL_IOMUX_FUNC_PWM1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLDOWN_ENABLE},
            };
        hal_gpio_pin_set_dir(PWM_BEEP, HAL_GPIO_DIR_OUT, 1);

        hal_iomux_init(pinmux_pwm, ARRAY_SIZE(pinmux_pwm));
        b_pwm_init_flag = true;
    }
}

int turn_passive_beep(int cmd, int freq)
{
    int range;
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = 8000,
        .ratio = 50,
        .inv = true,
        .sleep_on = true,
    };

    if (OFF == cmd) {
        hal_pwm_disable(HAL_PWM_ID_1);
    } else {
        if(freq<1000 || freq>5000) {
            printf("Beep set invalid PWM frequency[%d]!\n", freq);
            return -1;
        }
        /* Set PWM frequency */
        range=16000/freq;
        pwm_cfg.freq = range;
        /* Set PWM duty 50% */
        pwm_cfg.ratio = 0.5;
        hal_pwm_enable(HAL_PWM_ID_1, &pwm_cfg);
    }
    return 0;
}

static inline void play_tone(int tone, int delay)
{
    if(tone<DO || tone>RI1)
        return ;

    turn_passive_beep(ON, tone_freq[tone]);
    osDelay(delay);
    turn_passive_beep(OFF, 0);
}

void play_tiger(void)
{
    int i;

    for (i=0; i<ARRAY_SIZE(tiger_notation); i++) {
        play_tone(tiger_notation[i].tone, tiger_notation[i].delay_ms);
        osDelay(30);
    }

}

void pwm_beep_test(void)
{
    printf("enter %s %d ",__func__,__LINE__);
    pwm_init();
    //play_music();
    play_tiger();
}
