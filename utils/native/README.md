# utils/native<a name="EN-US_TOPIC_0000001148676553"></a>


-   [Introduction](#section11660541593)
-   [Directory Structure](#section17271017133915)
-   [Repositories Involved](#section1249817110914)

## Introduction<a name="section11660541593"></a>

The  **utils/native**  repository provides the following commonly used C++ utility classes:

-   Enhanced APIs for operations related to files, paths, and strings
-   APIs related to the read-write lock, semaphore, timer, thread, and thread pool
-   APIs related to the security data container and data serialization
-   Error codes for each subsystem
-   Safe functions in C

## Directory Structure<a name="section17271017133915"></a>

```
utils/native
└─ base
    ├── include       # Header files of APIs open to other subsystems
    ├── src           # Source files
    └── test          # Test code
```

## Repositories Involved<a name="section1249817110914"></a>

Utils subsystem

[utils](https://gitee.com/openharmony/utils/blob/master/README.md)

[utils\_native](https://gitee.com/openharmony/utils_native/blob/master/README.md)

[utils\_native\_lite](https://gitee.com/openharmony/utils_native_lite/blob/master/README.md)

