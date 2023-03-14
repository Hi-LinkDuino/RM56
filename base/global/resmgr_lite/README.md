# resmgr<a name="EN-US_TOPIC_0000001073289170"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1844145112017)
-   [Usage](#section17235135618274)
-   [Constraints](#section568761318105)
-   [Repositories Involved](#section15583142420413)

## Introduction<a name="section11660541593"></a>

The resource management module, namely, resmgr, provides the function of loading multi-language GUI resources for applications, for example, displaying the application names or icons specific to a certain language.

## Directory Structure<a name="section1844145112017"></a>

The directory structure for the resource management module is as follows:

```
/base/global/
├── resmgr_lite                # Code repository for the resource management module
│   ├──  frameworks            # Core code of the resource management module
│   │   ├── resmgr_lite        # Core code for resource parsing
│   │   │   ├── include        # Header files of the resource management module
│   │   │   ├── src            # Implementation code of the resource management module
│   │   │   └── test           # Test code
│   ├──  interfaces            # APIs of the resource management module
│   │   └── innerkits          # APIs of the resource management module for internal subsystems
```

## Usage<a name="section17235135618274"></a>

Call the  **GLOBAL\_GetValueById**  API to obtain the resource information of the application.

```
#include <stdint.h>
#include <stdio.h>
#include "global.h"

uint32_t id = 0x16777216;
char *values = NULL;
int32_t re = GLOBAL_GetValueById(id, "/system/data/resources.index", &values); // Obtain the resource of a specified application and write the resource to values.
std::count << values << std::endl;
if (values != NULL) {
    free(values);
}
```

## Constraints<a name="section568761318105"></a>

**Programming language**: C/C++

## Repositories Involved<a name="section15583142420413"></a>

[Globalization subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/globalization.md)

**global\_resmgr\_lite**

[global\_i18n\_lite](https://gitee.com/openharmony/global_i18n_lite/blob/master/README.md)

