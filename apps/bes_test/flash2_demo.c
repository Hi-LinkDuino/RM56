/**
 * Copyright (2019) BesTech. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
#include "app_utils.h"
#include "bes_test/bes_test_cases.h"
#include "beswifi.h"
#include "bwifi_interface.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "net_memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void demo_func_in_flash1();

void demo_func_in_flash2(void){
    TRACE(1, "%s: HEY, I am running in FLASH2", __func__);
    demo_func_in_flash1();
}

#endif