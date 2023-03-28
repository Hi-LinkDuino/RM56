#ifndef __LINUX_DEVICE_H__
#define __LINUX_DEVICE_H__

struct device {
    struct device		*parent;
    void    *platform_data;	/* Platform specific data, device*/
    void    *driver_data;
};

#endif
