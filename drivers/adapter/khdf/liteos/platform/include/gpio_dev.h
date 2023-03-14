/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GPIO_USER_H
#define GPIO_USER_H
#include "fcntl.h"
#include "hdf_base.h"
#include "sys/ioctl.h"

#define PL061_GPIO_USER_SUPPORT

int32_t GpioAddVfs(uint16_t bitNum);
void GpioRemoveVfs(void);

typedef void (*irq_func)(unsigned int irq, void *data);

typedef struct GpioBitInfo {
    unsigned int  groupnumber;
    unsigned int  bitnumber;

    unsigned char value;
#define GPIO_VALUE_HIGH         1
#define GPIO_VALUE_LOW          0
    unsigned char direction;
#define GPIO_DIRECTION_IN       0
#define GPIO_DIRECTION_IN_OUT   1

    unsigned char irq_status;
    unsigned char irq_enable;
#define GPIO_IRQ_ENABLE         1
#define GPIO_IRQ_DISABLE        0
    irq_func  irq_handler;
    unsigned int irq_type;
#define IRQ_TYPE_NONE           0x00000000
#define IRQ_TYPE_EDGE_RISING    0x00000001
#define IRQ_TYPE_EDGE_FALLING   0x00000002
#define IRQ_TYPE_EDGE_BOTH      (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define IRQ_TYPE_LEVEL_HIGH     0x00000004
#define IRQ_TYPE_LEVEL_LOW      0x00000008
#define IRQ_TYPE_LEVEL_MASK     (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH)
    void *data;
} gpio_groupbit_info;

#define GPIO_SET_DIR            _IOWR('w', 4, gpio_groupbit_info)
#define GPIO_GET_DIR            _IOWR('r', 5, gpio_groupbit_info)
#define GPIO_READ_BIT           _IOWR('r', 6, gpio_groupbit_info)
#define GPIO_WRITE_BIT          _IOWR('w', 7, gpio_groupbit_info)

#endif
