obj-y += port.o HAL_CM4.o

ccflags-y += -DOS_CLOCK_NOMINAL=$(OS_CLOCK_NOMINAL)

ifneq ($(OS_TASKCNT),)
ccflags-y += -DOS_TASKCNT=$(OS_TASKCNT)
endif

ifneq ($(OS_SCHEDULERSTKSIZE),)
ccflags-y += -DOS_SCHEDULERSTKSIZE=$(OS_SCHEDULERSTKSIZE)
endif

ifneq ($(OS_IDLESTKSIZE),)
ccflags-y += -DOS_IDLESTKSIZE=$(OS_IDLESTKSIZE)
endif
