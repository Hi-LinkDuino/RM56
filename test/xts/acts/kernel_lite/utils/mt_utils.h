/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// utils for multi-thread and multi-process test

#ifndef KERNEL_LITE_MT_UTILS
#define KERNEL_LITE_MT_UTILS

#include <stdint.h>

// count all primes numbers below 'maxNumber', used for keep cpu busy
int CountPrimes(uint32_t maxNumber);

/**
 * desc:   keep cpu run for 'ms' miliseconds, without any syscall or pend operation
 * input:  miliseconds value, only some specific value is supported, as below:
 *         -- 10, 20, 30, 60, 100, 150, 200, 300, 400, 600, 1000, 2000
 * output: useless, only used for avoid the whole function is optimized by gcc
 * note:   the actual elasped time is not so accurate
 */
int BusyRun(uint32_t ms);

/**
 * desc:   used for check if the code is runned as expected, in multi-thread or multi-process scenario
 * input:  step -- from 1 to 15(0xf).
 * output: the actual steps the this function is called.
 Example:
    LOG("step=%lx", CheckStep(1));  // may called in thread 1
    LOG("step=%lx", CheckStep(2));  // may called in thread 2
    LOG("step=%lx", CheckStep(3));  // may called in thread 3
    LOG("step=%lx", CheckStep(4));  // may called in thread 2
    uint64_t step = CheckStep(5);   // may called in thread 1
    if (step == 0x12345) {
        LOG("everything is ok");
    } else {
        LOG("code not run as expected!");
    }
 Output:
    step=1
    step=12
    step=123
    step=1234
    everything is ok
 */
uint64_t CheckStep(int step);


/**
 * desc:  used for synchronize in multi-process scenario, not suitable for multi-thread.
 * usage:
 InitPipe();
 fork();
 if parent:
    ...
    BlockOnPipe();  // will block until UnBlockPipe is called in another process.
    ...
 if child:
    ...
    UnBlockPipe();
    ...
 */
// return -1 if pipe-init failed, user should check return code
int InitPipe();
void BlockOnPipe();
void UnBlockPipe();

/**
 * desc:    get and set a global variable, can be used in multi-process scenario
 * usage:
 *    InitGlobalVariable();
 *    fork();
 *    if child:
 *        ...
 *        SetGlobalVariable(x);
 *        ...
 *    if parent:
 *        ...
 *        SetGlobalVariable(y);
 *        ...
 *        int i = GetGlobalVariable();
 *        // check if i is ok
 *        ...
 *    DeleteGlobalVariable(); // Call before exit
 *    exit...
 */
// output:  return -1 if the operation fails, user should check return code
int InitGlobalVariable();
// return -1 if the operation fails
int SetGlobalVariable(int value);
// return -1 if the operation fails.
int GetGlobalVariable();
// return -1 if the operation fails
int DeleteGlobalVariable();

#endif
