#ifndef __CSI_REPORTER_H__
#define __CSI_REPORTER_H__

#include <stdint.h>

#ifdef __cpulsplus
extern "C" {
#endif

void csi_reporter_task(void);

int csi_info_enqueue(const void *buf, uint32_t size);

#ifdef __cpulsplus
}
#endif

#endif
