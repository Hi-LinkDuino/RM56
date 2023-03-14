# Compile the test case

-   [Introduction]
-   [Directory]
-   [Constraints and Limitations]
-   [illustrate]
-   [Description of related issues]

## Introduction

Provides a framework based on cmake compilation, which can compile binary files for different platforms.

According to the needs of customers, compile and generate corresponding executable programs and library files. Its compilation process is:

1.  Use python script to configure the parameters of cmake compilation
2.  After python runs, a Makefile will be generated
3.  Execute the compiled task by running cmake --build .

## Directory

```
.
├── README.en.md
├── README.md
├── ndk-test
│   ├── CMakeLists.txt           # Starting point for ndk-test build via cmake
│   ├── build.py                 # Script to compile ndk-test
│   └── src
├── sanitize
│   ├── CMakeLists.txt           # Starting point for sanitize build via cmake
│   ├── asan
│   ├── build.py                 # Script to compile sanitize
│   ├── cfi
│   ├── fuzz
│   ├── scudo
│   └── ubsan
├── script                       # Script Directory
│   ├── __pycache__
│   ├── build.py                 # Total Compile Script
│   ├── config_args.py           # Parameters required for cmake compilation
│   ├── runtest-sanitize.sh
│   ├── runtest.sh
│   └── runtest_Windows.bat      # Push the compiled files to the board
└── target                       # save the target file
    └── README.md
```

## Constraints and Limitations

-   Need to download NDK or compile and generate through openharmony
-   Configuration environment, please refer to [help for wiki](https://gitee.com/openharmony/third_party_musl/wikis/Home)

## illustrate

1.  Compile commands for full compilation in the general directory
    
    ```
    python build.py
    ```

2.  The corresponding compilation directory can be compiled separately

    ```
    # subdirectory sanitize
    python build.py
    # subdirectory ndk-test
    python build.py
    ```

3.  Run the test case on the board

    ```
    # Make sure the board is connected to the computer and run the script
    runtest_Windows.bat
    ```

4.  If the above steps are executed successfully, the script will generate a REPORT and REPORT-SANITIZE files to record the results of running the ndk-test and sanitize test cases respectively

## Description of related issues

**For cmake parameter CMAKE_TOOCHAIN_FILE**
-   Points to the location of the toolchain file, which is important for compiling binaries for different platforms, so give the correct path