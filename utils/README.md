# utils<a name="EN-US_TOPIC_0000001078403148"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section17271017133915)
-   [Usage](#section137421544135015)
-   [Repositories Involved](#section1249817110914)

## Introduction<a name="section11660541593"></a>

The  **utils**  repository provides some commonly used C and C++ development enhancement APIs. 
It includes the following parts:

**C++ part:**

- Ability enhancement interface for file, path, string related operations;
- Interfaces such as read-write locks, semaphores, timers, thread enhancements and thread pools;
- Interfaces such as secure data container and data serialization;
- Definition of error codes for each subsystem;
- C language safety function interface.

**Part C:**

- HAL interface for standard file related operations;
- Some other internal functions, such as timers, etc. 

## Directory Structure<a name="section17271017133915"></a>

```
utils
├── native               # utils native implementation
└── system               # System-related predefined values and SA framework
```

## Usage<a name="section137421544135015"></a>

For details, see the API Reference.

## Repositories Involved<a name="section1249817110914"></a>

Utils subsystem

[utils](https://gitee.com/openharmony/utils/blob/master/README.md)

[utils\_native](https://gitee.com/openharmony/utils_native/blob/master/README.md)

[utils\_native\_lite](https://gitee.com/openharmony/utils_native_lite/blob/master/README.md)



