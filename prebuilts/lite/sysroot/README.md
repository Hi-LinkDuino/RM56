# sysroot<a name="EN-US_TOPIC_0000001096759155"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Constraints](#section119744591305)
-   [Compilation and Building](#section137768191623)
-   [Usage](#section68313135353)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

**sysroot**  is a root directory used by the Clang compiler to search for standard libraries and header files. The libc library is generated from the open-source musl library by compilation.

## Directory Structure<a name="section161941989596"></a>

```
/prebuilts/lite/sysroot
├── build                   # Toolchain building (including build scripts)
├── usr                     # C library and header files exposed externally
│   ├── include             # Header files exposed externally
│   │  └── arm-liteos       # Chip architecture for the toolchain
│   └── lib                 # C library exposed externally
│       └── arm-liteos      # Chip architecture for the toolchain
```

## Constraints<a name="section119744591305"></a>

**sysroot**  applies only to the OpenHarmony kernel.

## Compilation and Building<a name="section137768191623"></a>

When bugs in the musl library are fixed or the version is updated, you need to compile and build a new libc library by executing `make`  in the  **build**  directory, respectively. The new libc library will be stored in the  **/prebuilts/lite/sysroot/usr**  directory.

## Usage<a name="section68313135353"></a>

-   Add the bin directory to the PATH environment variable. For details about how to set up the compilation environment, see  [Quick start](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/Readme-EN.md).
-   The following is a sample script for compiling the  **helloworld.c**  program:

```
clang -o helloworld helloworld.c -target arm-liteos --sysroot=/my_ohos_root_path/prebuilts/lite/sysroot/
```

## Repositories Involved<a name="section1371113476307"></a>

[Kernel subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/kernel.md)

[prebuilts\_lite\_sysroot](https://gitee.com/openharmony/prebuilts_lite_sysroot/blob/master/README.md)

