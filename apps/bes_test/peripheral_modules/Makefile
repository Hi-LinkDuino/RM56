cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)

# select a target below to build
GPIO_PERIPH_TEST    ?= 0
SPI_PERIPH_TEST     ?= 0
UART_PERIPH_TEST    ?= 0
TIMER_PERIPH_TEST   ?= 0
DMA_PERIPH_TEST     ?= 0
PWM_PERIPH_TEST     ?= 0
I2C_PERIPH_TEST     ?= 0
WATCHDOG_PERIPH_TEST?= 0
I2S_PERIPH_TEST     ?= 0
# different target configurations
ifeq ($(GPIO_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DGPIO_PERIPH_TEST
obj-y += gpio/
endif

ifeq ($(SPI_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DSPI_PERIPH_TEST
obj-y += spi/
endif

ifeq ($(UART_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DUART_PERIPH_TEST
obj-y += uart/
endif

ifeq ($(TIMER_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DTIMER_PERIPH_TEST
obj-y += timer/
endif

ifeq ($(DMA_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DDMA_PERIPH_TEST
obj-y += dma/
endif

ifeq ($(PWM_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DPWM_PERIPH_TEST
obj-y += pwm/
endif

ifeq ($(I2C_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DI2C_PERIPH_TEST
obj-y += i2c/
endif

ifeq ($(WATCHDOG_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DWATCHDOG_PERIPH_TEST
obj-y += watchdog/
endif

ifeq ($(I2S_PERIPH_TEST),1)
KBUILD_CPPFLAGS += -DI2S_PERIPH_TEST
obj-y += i2s/
endif

ccflags-y += -Iapps/bes_test/ -Iapps/bes_test/peripheral_modules/
