/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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

/*
 *Description: Optimized ARM memmove and version of memcpy that can copy
 *overlapping data down
 */

/*
 *Local Configuration
 */

#define EXPORT_MEMMOVE_NAME          memmove
#define EXPORT_NORMAL_MEMCPY_NAME    memcpy_normal_x
#define EXPORT_FAST_MEMCPY_NAME      memcpy

#define MEMCPY_UNROLL_64BYTES

#ifdef MEMCPY_UNROLL_64BYTES
#define BLOCK_SIZE 16
#else
#define BLOCK_SIZE 8
#endif

#define CPY_STEP(inc)         \
        ldr    r2, [r3, r1] ; \
        str    r2, [r3], #(inc)


/* ========================================================= */
        .text
        .align    1
        .p2align  2,,3
        .arch     armv8-m.main
        .syntax   unified
        .thumb
        .thumb_func

/* ========================================================= */
        .global   EXPORT_FAST_MEMCPY_NAME
        .type     EXPORT_FAST_MEMCPY_NAME, %function
EXPORT_FAST_MEMCPY_NAME:
        cmp    r2, #3
        bls    .small_block
        b      .copy_end_down

        .size  EXPORT_FAST_MEMCPY_NAME, .-EXPORT_FAST_MEMCPY_NAME

        .p2align  2
/* ========================================================= */
        .global   EXPORT_MEMMOVE_NAME
        .type     EXPORT_MEMMOVE_NAME, %function
EXPORT_MEMMOVE_NAME:
        cmp    r2, #3
        bls    .small_block

        @ Check if we are moving up or down

        cmp    r1, r0        @ if src < dest, copy from end going down
        bls    .copy_end_down

        @ Here src >= dest, check overlap (src also less than dest_end)

        add    r3, r0, r2    @ Check overlap (r3 = dest_end)
        cmp    r1, r3        @ if src < dest_end, copy from start going up
        bls    .copy_start_up

        @ Moving down...
.copy_end_down:

        @ We know we have 4 bytes or more

        sub    ip, r2, #4
        ldr    r3, [r1, ip]  @ Copy tail
        str    r3, [r0, ip]

        sub    r1, r1, r0      @ Calculate source to dest distance

        cmp    r2, #8          @ Finished, if size is less than or equal to 8
        bls    .copy_dest_exit

        @ We have to write at least 1 byte, as aligned word

        and    r3, r0, #3      @ r3 = (dest & 3)
        rsb    r3, r3, #4      @ r3 = adjustment bytes (4,3,2,1)

        @ Calculate remaining block and end_p

        sub    r2, r2, r3      @ r2 = bytes remain = n - align
        lsr    r2, r2, #2      @ r2 = r2/4 = number of word remaining

        add    ip, r3, r0      @ Align dest pointer
        sub    ip, #4          @ dest_end_align (adjust for post decrement)
        add    r3, ip, r2, LSL #2  @ ip = dest_align + n_words - 1 word = dest_end_align

        @ r0 = dest
        @ r1 = src2dst dist (src - dest)
        @ r2 = n_words
        @ r3 = dest_end_align  (like r0, but aligned)
        @ ip = dest_start_p    (dest_align)

        and    r2, #BLOCK_SIZE-1
        tbb    [pc, r2]
.Ltab0:
        .byte    (.Lcpdloop   - .Ltab0)/2
        .byte    (.Lcpdloop1  - .Ltab0)/2
        .byte    (.Lcpdloop2  - .Ltab0)/2
        .byte    (.Lcpdloop3  - .Ltab0)/2
        .byte    (.Lcpdloop4  - .Ltab0)/2
        .byte    (.Lcpdloop5  - .Ltab0)/2
        .byte    (.Lcpdloop6  - .Ltab0)/2
        .byte    (.Lcpdloop7  - .Ltab0)/2
#ifdef MEMCPY_UNROLL_64BYTES
        .byte    (.Lcpdloop8  - .Ltab0)/2
        .byte    (.Lcpdloop9  - .Ltab0)/2
        .byte    (.Lcpdloop10 - .Ltab0)/2
        .byte    (.Lcpdloop11 - .Ltab0)/2
        .byte    (.Lcpdloop12 - .Ltab0)/2
        .byte    (.Lcpdloop13 - .Ltab0)/2
        .byte    (.Lcpdloop14 - .Ltab0)/2
        .byte    (.Lcpdloop15 - .Ltab0)/2
#endif

        @ BODY

        .p2align 2
.Lcpdloop:
#ifdef MEMCPY_UNROLL_64BYTES
.Lcpdloop16:     CPY_STEP(-4)
.Lcpdloop15:     CPY_STEP(-4)
.Lcpdloop14:     CPY_STEP(-4)
.Lcpdloop13:     CPY_STEP(-4)
.Lcpdloop12:     CPY_STEP(-4)
.Lcpdloop11:     CPY_STEP(-4)
.Lcpdloop10:     CPY_STEP(-4)
.Lcpdloop9:      CPY_STEP(-4)
#endif
.Lcpdloop8:      CPY_STEP(-4)
.Lcpdloop7:      CPY_STEP(-4)
.Lcpdloop6:      CPY_STEP(-4)
.Lcpdloop5:      CPY_STEP(-4)
.Lcpdloop4:      CPY_STEP(-4)
.Lcpdloop3:      CPY_STEP(-4)
.Lcpdloop2:      CPY_STEP(-4)
.Lcpdloop1:      CPY_STEP(-4)
        cmp    ip, r3
        bne    .Lcpdloop

.copy_dest_exit:
        ldr    r2, [r0, r1]  @ Copy head
        str    r2, [r0]
        bx     lr

/* ========================================================= */


        @ SMALL HEAD BLOCK

.small_block:
        cbz    r2, 2f

        mov    r3, r0        @ copy dest pointer
        add    ip, r2, r0    @ calculate end of dest
        ldr    r1, [r1]      @ load 4 bytes from src

        @ Now we know we have between 1 and 3 bytes

        .p2align 2
1:
        strb   r1, [r3], #1 @ store src byte to dest
        lsr    r1, #8       @ next src byte
        cmp    r3, ip
        bne    1b

2:
        bx     lr


/* ========================================================= */
        .align    1
        .p2align  2,,3
        .thumb_func

/* ========================================================= */
        .global   EXPORT_NORMAL_MEMCPY_NAME
        .type     EXPORT_NORMAL_MEMCPY_NAME, %function
EXPORT_NORMAL_MEMCPY_NAME:
        cmp    r2, #3
        bls    .small_block

        .p2align  2

/* ========================================================= */
        @ Moving up...
.copy_start_up:

        @ We know we have 4 bytes or more

        ldr    r3, [r1]        @ Copy head
        str    r3, [r0]

        cmp    r2, #8          @ Finished, if size is less than or equal to 8
        bls    .copy_tail_exit

        @ We have to write at least 1 byte, as aligned word

        push   {r2}            @ Save original length

        and    r3, r0, #3      @ r3 = (dest & 3)
        rsb    r3, r3, #4      @ r3 = adjustment bytes (4,3,2,1)

        @ Calculate remaining block and end_p

        sub    r2, r2, r3      @ r2 = bytes remain = n - align
        lsr    r2, r2, #2      @ r2 = r2/4 = number of word remaining

        add    r3, r3, r0      @ Align dest pointer
        sub    r1, r1, r0      @ Calculate source to dest distance
        add    ip, r3, r2, LSL #2  @ ip = p + 4*n_words = end_word_p

        @ r0 = dest
        @ r1 = src2dst dist (src - dest)
        @ r2 = n_words
        @ r3 = dest_align   (like r0, but aligned)
        @ ip = dest_end_p   (dest_align + n_words)

        and    r2, #BLOCK_SIZE-1
        tbb    [pc, r2]
.Ltab1:
        .byte    (.Lcpaloop   - .Ltab1)/2
        .byte    (.Lcpaloop1  - .Ltab1)/2
        .byte    (.Lcpaloop2  - .Ltab1)/2
        .byte    (.Lcpaloop3  - .Ltab1)/2
        .byte    (.Lcpaloop4  - .Ltab1)/2
        .byte    (.Lcpaloop5  - .Ltab1)/2
        .byte    (.Lcpaloop6  - .Ltab1)/2
        .byte    (.Lcpaloop7  - .Ltab1)/2
#ifdef MEMCPY_UNROLL_64BYTES
        .byte    (.Lcpaloop8  - .Ltab1)/2
        .byte    (.Lcpaloop9  - .Ltab1)/2
        .byte    (.Lcpaloop10 - .Ltab1)/2
        .byte    (.Lcpaloop11 - .Ltab1)/2
        .byte    (.Lcpaloop12 - .Ltab1)/2
        .byte    (.Lcpaloop13 - .Ltab1)/2
        .byte    (.Lcpaloop14 - .Ltab1)/2
        .byte    (.Lcpaloop15 - .Ltab1)/2
#endif
        @ BODY

        .p2align 2
.Lcpaloop:
#ifdef MEMCPY_UNROLL_64BYTES
.Lcpaloop16:     CPY_STEP(4)
.Lcpaloop15:     CPY_STEP(4)
.Lcpaloop14:     CPY_STEP(4)
.Lcpaloop13:     CPY_STEP(4)
.Lcpaloop12:     CPY_STEP(4)
.Lcpaloop11:     CPY_STEP(4)
.Lcpaloop10:     CPY_STEP(4)
.Lcpaloop9:      CPY_STEP(4)
#endif
.Lcpaloop8:      CPY_STEP(4)
.Lcpaloop7:      CPY_STEP(4)
.Lcpaloop6:      CPY_STEP(4)
.Lcpaloop5:      CPY_STEP(4)
.Lcpaloop4:      CPY_STEP(4)
.Lcpaloop3:      CPY_STEP(4)
.Lcpaloop2:      CPY_STEP(4)
.Lcpaloop1:      CPY_STEP(4)
        cmp    ip, r3
        bne    .Lcpaloop

        pop    {r2}          @ get original length
        add    r1, r1, r0    @ src = r1 + dest

.copy_tail_exit:
        sub    ip, r2, #4
        ldr    r3, [r1, ip]  @ Copy tail
        str    r3, [r0, ip]
        bx     lr

        .size  EXPORT_MEMMOVE_NAME, .-EXPORT_MEMMOVE_NAME

