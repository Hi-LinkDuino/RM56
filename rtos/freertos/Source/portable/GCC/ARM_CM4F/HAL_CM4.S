/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    HAL_CM4.S
 *      Purpose: Hardware Abstraction Layer for Cortex-M4
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2013 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

        #include "cmsis.h"

        .file   "HAL_CM4.S"
        .syntax unified

/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

        .thumb

        .section ".text"
        .align  2
/*--------------------------- _alloc_box ------------------------------------*/

#      void *_alloc_box (void *box_mem);
       /* Function wrapper for Unprivileged/Privileged mode. */

        .thumb_func
        .type   _alloc_box, %function
        .global _alloc_box
_alloc_box:
        .fnstart
        .cantunwind

        LDR     R12,=rt_alloc_box
        MRS     R3,IPSR
        LSLS    R3,#24
        IT      NE
        BXNE    R12
        MRS     R3,CONTROL
        LSLS    R3,#31
        IT      EQ
        BXEQ    R12
        SVC     0
        BX      LR

        .fnend
        .size   _alloc_box, .-_alloc_box


/*--------------------------- _free_box -------------------------------------*/

#       int _free_box (void *box_mem, void *box);
        /* Function wrapper for Unprivileged/Privileged mode. */

        .thumb_func
        .type   _free_box, %function
        .global _free_box
_free_box:
        .fnstart
        .cantunwind

        LDR     R12,=rt_free_box
        MRS     R3,IPSR
        LSLS    R3,#24
        IT      NE
        BXNE    R12
        MRS     R3,CONTROL
        LSLS    R3,#31
        IT      EQ
        BXEQ    R12
        SVC     0
        BX      LR

        .fnend
        .size   _free_box, .-_free_box

        .end

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
