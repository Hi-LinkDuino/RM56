#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "errno.h"
#include "reg_sdmmcip.h"
#include "cmsis_nvic.h"
#include "hal_sdio.h"
#include "hal_sdmmc.h"
#include "hal_uart.h"
#include "hal_trace.h"



int32_t sdio_io_rw_extended_block(struct sdio_function *func,
                                            int32_t               rw,
                                            uint32_t              addr,
                                            int32_t               op_code,
                                            uint8_t              *buf,
                                            uint32_t              len)
{
    
    return 0;
}



int32_t sdio_io_read_block(struct sdio_function *func,
                                     uint32_t              addr,
                                     uint8_t              *buf,
                                     uint32_t              len)
{
    return sdio_io_rw_extended_block(func, 0, addr, 1, buf, len);
}

int32_t sdio_io_write_block(struct  sdio_function *func,
                                      uint32_t              addr,
                                      uint8_t              *buf,
                                      uint32_t              len)
{
    return sdio_io_rw_extended_block(func, 1, addr, 1, buf, len);
}




/**
 *	sdio_align_size - pads a transfer size to a more optimal value
 *	@func: SDIO function
 *	@sz: original transfer size
 *
 *	Pads the original data size with a number of extra bytes in
 *	order to avoid controller bugs and/or performance hits
 *	(e.g. some controllers revert to PIO for certain sizes).
 *
 *	If possible, it will also adjust the size so that it can be
 *	handled in just a single request.
 *
 *	Returns the improved size, which might be unmodified.
 */
unsigned int sdio_align_size(struct sdio_function *func, unsigned int sz)
{
	
	return 0;
}


void hal_sdio_enable_irq(struct sdio_function *func)
{
   return;
}

void hal_sdio_disable_irq(struct sdio_function *func)
{
   return;
}

int32_t sdio_enable_func(struct sdio_function *func)
{
     return 0;
}

int32_t sdio_disable_func(struct sdio_function *func)
{
     return 0;

}

int32_t sdio_set_block_size(struct sdio_function *func,
                               uint32_t              blksize)
{
    
    return 0;
}


int32_t sdio_attach_irq(struct sdio_function *func,
                           sdio_irq_handler_t   handler)
{
    int32_t ret = 0;

    ASSERT(func != NULL, "func != NULL\n");


    TRACE(0, "SDIO: enabling IRQ for function %d\n", func->num);
    
   // TRACE("SDIO: enabling IRQ for function 0x%x\n", func);
  //  return 0;

    if (func->irq_handler)
    {
        TRACE(0, "SDIO: IRQ for already in use.\n");

        return -EBUSY;
    }


    func->irq_handler = handler;

   
    TRACE(0, "sdio_attach_irq ret = %d handler=0x%x\n", ret, (unsigned int)handler);
    return ret;
}

int32_t sdio_detach_irq(struct sdio_function *func)
{
    ASSERT(func != NULL, "func != NULL\n");

  //  func0 = &func->mmc->sdio_function[0];

    TRACE(0, "SDIO: disabling IRQ for function %d\n", func->num);

   // NVIC_DisableIRQ(SDIO_IRQn);

    if (func->irq_handler)
    {
        func->irq_handler = NULL;
    }

    return 0;
}


