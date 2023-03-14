#ifndef CONFIG_JUPITER_H
#define CONFIG_JUPITER_H

#ifdef JERRY_FOR_IAR_CONFIG

#ifdef JERRY_IAR_JUPITER

#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include "dirent.h"
#include "ohos_types.h"

// Maximum size for js and snapshot file
#ifndef INPUTJS_BUFFER_SIZE
#define INPUTJS_BUFFER_SIZE (32 * 1024)
#endif /* INPUTJS_BUFFER_SIZE */

#ifndef SNAPSHOT_BUFFER_SIZE
#define SNAPSHOT_BUFFER_SIZE (24 * 1024)
#endif /* SNAPSHOT_BUFFER_SIZE */

#define CONVERTION_RATIO (1024)
#define CONTEXT_SIZE_FOR_TASK (3)

#ifndef BMS_TASK_HEAP_SIZE
#define BMS_TASK_HEAP_SIZE (48)
#endif /* BMS_TASK_HEAP_SIZE */

#define BMS_TASK_CONTEXT_AND_HEAP_SIZE (BMS_TASK_HEAP_SIZE + CONTEXT_SIZE_FOR_TASK)

#ifndef JS_TASK_HEAP_SIZE
#define JS_TASK_HEAP_SIZE (64)
#endif /* JS_TASK_HEAP_SIZE */

// >= 51400 + (64-48) * 1024 + 8
#define JS_TASK_CONTEXT_AND_HEAP_SIZE_BYTE (JS_TASK_HEAP_SIZE * CONVERTION_RATIO + 2248 + 16)

#endif /* JERRY_IAR_JUPITER */

#endif /* JERRY_FOR_IAR_CONFIG */

#endif // CONFIG_JUPITER_H
