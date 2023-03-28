/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "app_key.h"
#include "alsa.h"
#include "hal_trace.h"
#include "hal_spi.h"
#include "hal_iomux.h"
#include "string.h"
#include "cmsis.h"
#include "cmsis_os.h"

static int key_code[] =
{
   0,    //KEYBOARD_VOL_INC_SHORT
   1,    //KEYBOARD_VOL_DEC_SHORT
   2,    //KEYBOARD_MIC_SHORT
   3,    //KEYBOARD_VOL_INC_LONG
   4,    //KEYBOARD_VOL_DEC_LONG
   5,    //KEYBOARD_MIC_LONG
   6,    //KEYBOARD_MIC_VOL_INC
   7,    //KEYBOARD_MIC_VOL_DEC
   8,    //KEYBOARD_AUTO_SHORT
   9,    //KEYBOARD_AUTO_LONG
   10,   //KEYBOARD_AUTO_VOL_INC
   11,   //KEYBOARD_AUTO_VOL_DEC
   12,   //KEYBOARD_MIC_AUTO
   13,   //KEYBOARD_PHONE_HANGUP,
};

static void __bes_key_proc(APP_KEY_STATUS *status, void *param)
{
    int key_code = *((int *)param);

    switch (key_code)
    {
        case 0:
        {
#ifdef __ALSA_EN__
            int vol = alsa_volume_get();
            vol++;
            if ( vol <= 17 )
                alsa_volume_set(vol);
#endif
        }
        break;
        case 1:
        {
#ifdef __ALSA_EN__
            int vol = alsa_volume_get();
            vol --;
            if( vol >= 1 )
                alsa_volume_set(vol);
#endif
        }
        break;
        default:
            break;
    }
    TRACE(0, "%s code=%d, event=%d -> key_code=%d",__func__, status->code, status->event, key_code);
}

static const APP_KEY_HANDLE _bes_key_handle_cfg[] =
{
    {{APP_KEY_CODE_FN1,                  APP_KEY_EVENT_CLICK        },     "KEYBOARD_VOL_INC_SHORT",  __bes_key_proc, (void *)key_code      },
    {{APP_KEY_CODE_FN3,                  APP_KEY_EVENT_CLICK        },     "KEYBOARD_VOL_DEC_SHORT",  __bes_key_proc, (void *)(key_code+1)  },
    {{APP_KEY_CODE_FN2,                  APP_KEY_EVENT_CLICK        },     "KEYBOARD_MIC_SHORT"    ,  __bes_key_proc, (void *)(key_code+2)  },
    {{APP_KEY_CODE_FN1,                  APP_KEY_EVENT_REPEAT       },     "KEYBOARD_VOL_INC_LONG" ,  __bes_key_proc, (void *)(key_code+3)  },
    {{APP_KEY_CODE_FN3,                  APP_KEY_EVENT_REPEAT       },     "KEYBOARD_VOL_DEC_LONG" ,  __bes_key_proc, (void *)(key_code+4)  },
    {{APP_KEY_CODE_FN2,                  APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_MIC_LONG"     ,  __bes_key_proc, (void *)(key_code+5)  },
    {{APP_KEY_CODE_FN2|APP_KEY_CODE_FN1, APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_MIC_VOL_INC"  ,  __bes_key_proc, (void *)(key_code+6)  },
    {{APP_KEY_CODE_FN2|APP_KEY_CODE_FN3, APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_MIC_VOL_DEC"  ,  __bes_key_proc, (void *)(key_code+7)  },
    {{APP_KEY_CODE_FN6,                  APP_KEY_EVENT_CLICK        },     "KEYBOARD_AUTO_SHORT"   ,  __bes_key_proc, (void *)(key_code+8)  },
    {{APP_KEY_CODE_FN6,                  APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_AUTO_LONG"    ,  __bes_key_proc, (void *)(key_code+9)  },
    {{APP_KEY_CODE_FN1|APP_KEY_CODE_FN6, APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_AUTO_VOL_INC" ,  __bes_key_proc, (void *)(key_code+10) },
    {{APP_KEY_CODE_FN3|APP_KEY_CODE_FN6, APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_AUTO_VOL_DEC" ,  __bes_key_proc, (void *)(key_code+11) },
    {{APP_KEY_CODE_FN2|APP_KEY_CODE_FN6, APP_KEY_EVENT_LONGLONGPRESS},     "KEYBOARD_MIC_AUTO"     ,  __bes_key_proc, (void *)(key_code+12) },
    {{APP_KEY_CODE_FN2,                  APP_KEY_EVENT_LONGPRESS    },     "KEYBOARD_PHONE_HANGUP" ,  __bes_key_proc, (void *)(key_code+13) },
};

int register_keyboard_event( void )
{
    int ret = 0;

    app_key_handle_clear();
    for (int i=0; i<(sizeof(_bes_key_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++)
        app_key_handle_registration(&_bes_key_handle_cfg[i]);

    return ret;
} 
int bes_key_test(void)
{
    register_keyboard_event();
    return 0;
}

/*
static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi1[] = {
    {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};
*/

static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd[] = {
	{HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
};

int bes_spi_clk_test()
{
    TRACE(0, "\n\n%-32s testing......\n", __FUNCTION__);
#if 0
    struct HAL_SPI_CFG_T spi_cfg;
    uint32_t spi_data = 0;
    memset(&spi_cfg, 0, sizeof(spi_cfg));

    hal_iomux_init(&pinmux_spilcd[0], sizeof(pinmux_spilcd)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    //hal_iomux_set_spilcd();

    spi_cfg.clk_delay_half = 0;
    spi_cfg.clk_polarity = 0;
    spi_cfg.slave = 0;
    spi_cfg.dma_rx = 0;
    spi_cfg.dma_tx = 0;
    spi_cfg.rx_sep_line = 0;
    spi_cfg.cs = 0;
    spi_cfg.tx_bits = 28;
    spi_cfg.rx_bits = 24;
    spi_cfg.rx_frame_bits = 32;
    spi_cfg.rate = 80*1000*1000;

    hal_spilcd_open(&spi_cfg);

    spi_data = 0x12345678;
    while(1) {
        hal_spilcd_send(&spi_data, 4);
        osDelay(10);
	}
#endif
    return 0;
}


int bes_spi_lcd_test()
{
    TRACE(0, "\n\n%-32s testing......\n", __FUNCTION__);
#ifdef USED_GUI
    extern void gui_main(void);
    gui_main();
#endif
    return 0;
}