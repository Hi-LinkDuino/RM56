
obj-y := list.o queue.o tasks.o timers.o stream_buffer.o event_groups.o
obj-y += portable/MemMang/

ifeq ($(CPU),m33)
obj-y += portable/GCC/ARM_CM33_NTZ/
else
obj-y += portable/GCC/ARM_CM4F/
#obj-y += portable/GCC/ARM_CM4_MPU/
endif

