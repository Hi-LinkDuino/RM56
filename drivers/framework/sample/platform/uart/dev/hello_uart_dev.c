/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "hdf_log.h"

#define HDF_LOG_TAG hello_uart
#define INFO_SIZE 16

int main(void)
{
    int ret;
    int fd;
    const char info[INFO_SIZE] = {" HELLO UART! "};

    fd = open("/dev/uartdev-5", O_RDWR);
    if (fd < 0) {
        HDF_LOGE("uartdev-5 open failed %d", fd);
        return -1;
    }
    ret = write(fd, info, INFO_SIZE);
    if (ret != 0) {
        HDF_LOGE("write uartdev-5 ret is %d", ret);
    }
    ret = close(fd);
    if (ret != 0) {
        HDF_LOGE("uartdev-5 close failed %d", fd);
        return -1;
    }
    return ret;
}