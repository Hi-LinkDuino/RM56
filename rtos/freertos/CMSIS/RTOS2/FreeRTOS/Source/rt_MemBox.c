/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX.C
 *      Purpose: Interface functions for fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2012 ARM Germany GmbH
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

#include "stdint.h"
#include "rt_MemBox.h"
#include "cmsis_os.h"
#include "task.h"

extern void *_alloc_box (void *box_mem);
extern int  _free_box (void *box_mem, void *box);

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- _init_box -------------------------------------*/

int _init_box  (void *box_mem, uint32_t box_size, uint32_t blk_size) {
  /* Initialize memory block system, returns 0 if OK, 1 if fails. */
  void *end;
  void *blk;
  void *next;
  uint32_t  sizeof_bm;
  int  irq_dis = 0;

  /* Create memory structure. */
  if (blk_size & BOX_ALIGN_8) {
    /* Memory blocks 8-byte aligned. */
    blk_size = ((blk_size & ~BOX_ALIGN_8) + 7) & ~7;
    sizeof_bm = (sizeof (struct OS_BM) + 7) & ~7;
  }
  else {
    /* Memory blocks 4-byte aligned. */
    blk_size = (blk_size + 3) & ~3;
    sizeof_bm = sizeof (struct OS_BM);
  }
  if (blk_size == 0) {
    return (1);
  }
  if ((blk_size + sizeof_bm) > box_size) {
    return (1);
  }


  //irq_dis = __disable_irq ();
  if (IN_ISR())
    irq_dis = taskENTER_CRITICAL_FROM_ISR();
  else
    taskENTER_CRITICAL();

  /* Create a Memory structure. */
  blk = ((uint8_t *) box_mem) + sizeof_bm;
  ((P_BM) box_mem)->free = blk;
  end = ((uint8_t *) box_mem) + box_size;
  ((P_BM) box_mem)->end      = end;
  ((P_BM) box_mem)->blk_size = blk_size;

  /* Link all free blocks using offsets. */
  end = ((uint8_t *) end) - blk_size;
  while (1)  {
    next = ((uint8_t *) blk) + blk_size;
    if (next > end)  break;
    *((void **)blk) = next;
    blk = next;
  }
  /* end marker */
  *((void **)blk) = 0;

  if (IN_ISR())
    taskEXIT_CRITICAL_FROM_ISR(irq_dis);
  else
    taskEXIT_CRITICAL();
  return (0);
}

/*--------------------------- rt_alloc_box ----------------------------------*/

void *rt_alloc_box (void *box_mem) {
  /* Allocate a memory block and return start address. */
  void **free;
#ifndef __USE_EXCLUSIVE_ACCESS
  int  irq_dis = 0;

  //irq_dis = __disable_irq ();
  if (IN_ISR())
    irq_dis = taskENTER_CRITICAL_FROM_ISR();
  else
    taskENTER_CRITICAL();

  free = ((P_BM) box_mem)->free;
  if (free) {
    ((P_BM) box_mem)->free = *free;
  }
  //if (!irq_dis) __enable_irq ();
  if (IN_ISR())
    taskEXIT_CRITICAL_FROM_ISR(irq_dis);
  else
    taskEXIT_CRITICAL();

#else
  do {
    if ((free = (void **)__ldrex(&((P_BM) box_mem)->free)) == 0) {
      __clrex();
      break;
    }
  } while (__strex((uint32_t)*free, &((P_BM) box_mem)->free));
#endif
  return (free);
}


/*--------------------------- _calloc_box -----------------------------------*/

void *_calloc_box (void *box_mem)  {
  /* Allocate a 0-initialized memory block and return start address. */
  void *free;
  uint32_t *p;
  uint32_t i;

  free = _alloc_box (box_mem);
  if (free)  {
    p = free;
    for (i = ((P_BM) box_mem)->blk_size; i; i -= 4)  {
      *p = 0;
      p++;
    }
  }
  return (free);
}


/*--------------------------- rt_free_box -----------------------------------*/

int rt_free_box (void *box_mem, void *box) {
  /* Free a memory block, returns 0 if OK, 1 if box does not belong to box_mem */
#ifndef __USE_EXCLUSIVE_ACCESS
  int irq_dis = 0;
#endif

  if (box < box_mem || box >= ((P_BM) box_mem)->end) {
   // ASSERT(0, "free box error, box_mem %p, box %p, end %p", box_mem, box,
    //                ((P_BM) box_mem)->end);
    return (1);
  }

#ifndef __USE_EXCLUSIVE_ACCESS
  //irq_dis = __disable_irq ();
  if (IN_ISR())
    irq_dis = taskENTER_CRITICAL_FROM_ISR();
  else
    taskENTER_CRITICAL();

  *((void **)box) = ((P_BM) box_mem)->free;
  ((P_BM) box_mem)->free = box;
 // if (!irq_dis) __enable_irq ();
  if (IN_ISR())
      taskEXIT_CRITICAL_FROM_ISR(irq_dis);
  else
      taskEXIT_CRITICAL();

#else
  do {
    *((void **)box) = (void *)__ldrex(&((P_BM) box_mem)->free);
  } while (__strex ((uint32_t)box, &((P_BM) box_mem)->free));
#endif
  return (0);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

