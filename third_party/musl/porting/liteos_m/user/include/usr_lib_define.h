/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd. All rights reserved.
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

#ifndef __USR_LIBC_DEFINE_H
#define __USR_LIBC_DEFINE_H

#define _LIBC_TEXT_ENTRY   __attribute__ ((section(".usr_libc_entry")))
#define _LIBC_TEXT_SECTION __attribute__ ((section(".usr_libc_text")))
#define _LIBC_DATA_SECTION __attribute__ ((section(".usr_libc_data")))

#define pthread_attr_init            b_pthread_attr_init
#define pthread_attr_destroy         b_pthread_attr_destroy
#define pthread_attr_setdetachstate  b_pthread_attr_setdetachstate
#define pthread_attr_getdetachstate  b_pthread_attr_getdetachstate
#define pthread_attr_setscope        b_pthread_attr_setscope
#define pthread_attr_getscope        b_pthread_attr_getscope
#define pthread_attr_setinheritsched b_pthread_attr_setinheritsched
#define pthread_attr_getinheritsched b_pthread_attr_getinheritsched
#define pthread_attr_setschedpolicy  b_pthread_attr_setschedpolicy
#define pthread_attr_getschedpolicy  b_pthread_attr_getschedpolicy
#define pthread_attr_setschedparam   b_pthread_attr_setschedparam
#define pthread_attr_getschedparam   b_pthread_attr_getschedparam
#define pthread_attr_setstack        b_pthread_attr_setstack
#define pthread_attr_getstack        b_pthread_attr_getstack
#define pthread_attr_setstacksize    b_pthread_attr_setstacksize
#define pthread_attr_getstacksize    b_pthread_attr_getstacksize
#define sched_get_priority_min       b_sched_get_priority_min
#define sched_get_priority_max       b_sched_get_priority_max
#define pthread_create               b_pthread_create
#define pthread_setschedparam        b_pthread_setschedparam
#define pthread_getschedparam        b_pthread_getschedparam
#define pthread_self                 b_pthread_self
#define pthread_cancel               b_pthread_cancel
#define pthread_join                 b_pthread_join
#define pthread_detach               b_pthread_detach
#define pthread_exit                 b_pthread_exit
#define pthread_setname_np           b_pthread_setname_np
#define pthread_getname_np           b_pthread_getname_np

#define memcpy b_memcpy
#define memcmp b_memcmp
#define memset b_memset
#define strlen b_strlen
#define strcmp b_strcmp

#endif
