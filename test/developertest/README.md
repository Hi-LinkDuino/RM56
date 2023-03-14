# Developers Test<a name="EN-US_TOPIC_0000001122338419"></a>

-   [Overview](#section7375710115617)
-   [Directory Structure](#section102031353175317)
-   [Constraints](#section87444710110)
-   [Installation](#section1347156474)
-   [Using Test Framework](#section75882026185016)
-   [Test Result and Log](#section414715805819)
-   [Repositories Involved](#section6299103515474)

## Overview<a name="section7375710115617"></a>

This module allows you to develop new test cases for new features, or modify existing test cases for modified features. The developers test framework helps you develop high-quality code.

## Directory Structure<a name="section102031353175317"></a>

```
developertest/
├── aw                            # Static libraries of the test framework
│   ├── cxx                      # C++ libraries
│   └── python                   # Python libraries
├── config                        # Test framework configuration
│   ├── build_config.xml         # Test case build configuration
│   ├── filter_config.xml        # Test case filter configuration
│   ├── framework_config.xml     # Test type configuration
│   └── user_config.xml          # User configuration
├── examples                      # Test case examples
│   ├── calculator               # Calculator
│   └── test                     # Test resources
├── src                           # Source code of the test framework
│   ├── main                     # Entry function
│   └── core                     # Core code of the test framework
├── third_party                   # Adaptation code for third-party components on which the test framework relies
│   └── lib                      # Compilation configuration of the static library
├── BUILD.gn                      # Compilation entry of the test framework
├── start.bat                     # Developers test entry for Windows
└── start.sh                      # Developers test entry for Linux
```

## Constraints<a name="section87444710110"></a>

The test tool environment must meet the following requirements:

1.  Python version: 3.7.5 or later
2.  Paramiko version: 2.7.1 or later
3.  Setuptools version: 40.8.0 or later
4.  RSA version: 4.0 or later
5.  NFS version: V4 or later \(required when device supports connection using the serial port but not the hdc\)
6.  pySerial version: 3.3 or later \(required when the device supports connection using the serial port but not the hdc\)
7.  OS version: Windows 10 or later; Ubuntu 18.04

## Installation<a name="section1347156474"></a>

The Python environment is required.

1.  Run the following command to install the Linux extension component Readline:

    ```
    sudo apt-get install libreadline-dev
    ```

    ```
    Reading package lists... Done
    Building dependency tree
    Reading state information... Done
    libreadline-dev is already the newest version (7.0-3).
    0 upgraded, 0 newly installed, 0 to remove and 11 not upgraded.
    ```

2.  Run the following command to install the plug-in Setuptools:

    ```
    pip3 install setuptools
    ```

    If the installation is successful, the following prompts are displayed:

    ```
    Requirement already satisfied: setuptools in d:\programs\python37\lib\site-packages (41.2.0)
    ```

3.  Run the following command to install the plug-in Paramiko:

    ```
    pip3 install paramiko
    ```

    If the installation is successful, the following prompts are displayed:

    ```
    Installing collected packages: pycparser, cffi, pynacl, bcrypt, cryptography, paramiko
    Successfully installed bcrypt-3.2.0 cffi-1.14.4 cryptography-3.3.1 paramiko-2.7.2 pycparser-2.20 pynacl-1.4.0
    ```

4.  Run the following command to install the Python plug-in RSA:

    ```
    pip3 install rsa
    ```

    If the installation is successful, the following prompts are displayed:

    ```
    Installing collected packages: pyasn1, rsa
    Successfully installed pyasn1-0.4.8 rsa-4.7
    ```

5.  Run the following command to install the serial port plug-in pySerial for Python on the local PC:

    ```
    pip3 install pyserial
    ```

    If the installation is successful, the following prompts are displayed:

    ```
    Requirement already satisfied: pyserial in d:\programs\python37\lib\site-packages\pyserial-3.4-py3.7.egg (3.4)
    ```

6.  If the device supports test result output only using the serial port, install the NFS server.

    For example, to install haneWIN NFS Server 1.2.50 for Windows, download the installation package from https://www.hanewin.net/nfs-e.htm.

    For Linux, run the following command:

    ```
    sudo apt install nfs-kernel-server
    ```

    If the installation is successful, the following prompts are displayed:

    ```
    Reading package lists... Done
    Building dependency tree
    Reading state information... Done
    nfs-kernel-server is already the newest version (1:1.3.4-2.1ubuntu5.3).
    0 upgraded, 0 newly installed, 0 to remove and 11 not upgraded.
    ```


## Using Test Framework<a name="section75882026185016"></a>

-   \(Optional\) Install the XDevice module.
    1.  Go to the XDevice installation directory  **test/xdevice**.
    2.  Open the console and run the following command:

        ```
        python setup.py install
        ```

        If the installation is successful, the following prompts are displayed:

        ```
        Installed d:\programs\python37\lib\site-packages\xdevice-0.0.0-py3.7.egg
        Processing dependencies for xdevice==0.0.0
        Finished processing dependencies for xdevice==0.0.0
        ```


-   Configure the developers test module in the configuration file  **developertest/config/user\_config.xml**.
    1.  Modify basic configuration parameters of the test framework.

        \[build\]    \# Configure build parameters of the test case.

        ```
        <build>
            <example>false</example>
            <version>false</version>
            <testcase>true</testcase>
            ...
        </build>
        ```

        >![](public_sys-resources/icon-note.gif) **NOTE:** 
        >**example**: whether to build the test case example. The default value is  **false**.
        >**version**: whether to build the test version. The default value is  **false**.
        >**testcase**: whether to build the test case. The default value is  **true**.

    2.  For devices that support the Harmony device connector \(hdc\), modify the configuration file as follows:

        \[device\]    \# Configure the device information with the  **"usb-hdc"**  attribute, including the test device IP address and the matched hdc port.

        ```
        <device type="usb-hdc">
            <ip>192.168.1.1</ip>
            <port>9111</port>
            <sn></sn>
        </device>
        ```

    3.  For devices that only support the serial port connection, modify the configuration file as follows:

        \[device\]    \# Configure the serial port information with the  **"ipcamera"**  attribute, including the COM port and baud rate.

        ```
        <device type="com" label="ipcamera">
            <serial>
                <com>COM1</com>
                <type>cmd</type>
                <baud_rate>115200</baud_rate>
                <data_bits>8</data_bits>
                <stop_bits>1</stop_bits>
                <timeout>1</timeout>
            </serial>
        </device>
        ```


-   \(Optional\) Modify the configuration file  **config/user\_config.xml**  of the  **developertest**  module. If a test case has been compiled, specify the compilation output directory of the test case. In this case, the test platform will not recompile the test case.
    1.  Specify the output directory of the test case, that is, the compilation output directory between the  **<test\_cases\>**  tags. 

        ```
        <test_cases>
            <dir>/home/opencode/out/release/tests</dir>
        </test_cases>
        ```

    2.  For devices that only support serial port connection, specify the NFS directory on the PC \(**host\_dir**\) and the corresponding directory on the board \(**board\_dir**\) between the  **<NFS\>**  tags. For example:

        ```
        <NFS>
            <host_dir>D:\nfs</host_dir>
            <board_dir>user</board_dir>
        </NFS>
        ```


-   Prepare the test environment. \(Check that the test environment is ready if the tested device only supports serial port connection.\)
    -   The system image and file system have been burnt into a development board and are running properly on the development board. For example, in system mode, device prompt  **OHOS\#**  is displayed during shell login, indicating that the system is running properly.
    -   The development host has been connected to the serial port of the development board and the network port.
    -   The IP addresses of the development host and development board are in the same network segment and can ping each other.
    -   An empty directory is created on the development host for mounting test cases through the NFS, and the NFS service is started properly.

-   Run test suites.
    -   Start the test framework and go to the  **test/developertest**  directory.
        1.  Run the following command to start the test framework in Windows.

            ```
            start.bat
            ```

        2.  Run the following command to start the test framework in Linux.

            ```
            ./strat.sh
            ```


    -   Select a device form.

        Configure device forms based on the actual development board, for example,  **developertest/config/framework\_config.xml**.

    -   Run test commands.
        1.  To query the subsystems, modules, product forms, and test types supported by test cases, run the  **show**  commands.

            ```
            usage: 
                show productlist      Querying Supported Product Forms
                show typelist         Querying the Supported Test Type
                show subsystemlist    Querying Supported Subsystems
                show modulelist       Querying Supported Modules
            ```

        2.  Run the execution command, where  **-t**  is mandatory, and  **-ss**  and  **-tm**  are optional.

            ```
            run -t ut -ss test -tm example
            ```

        3.  Specify the parameters that can be used to execute the test suite corresponding to a specific feature or module.

            ```
            usage: run [-h] [-p PRODUCTFORM] [-t [TESTTYPE [TESTTYPE ...]]]
                [-ss SUBSYSTEM] [-tm TESTMODULE] [-ts TESTSUIT]
                [-tc TESTCASE] [-tl TESTLEVEL] 
            
            optional arguments:
                -h, --help            show this help message and exit
                -p PRODUCTFORM, --productform PRODUCTFORM    Specified product form
                -t [TESTTYPE [TESTTYPE ...]], --testtype [TESTTYPE [TESTTYPE ...]]
                    Specify test type(UT,MST,ST,PERF,ALL)
                -ss SUBSYSTEM, --subsystem SUBSYSTEM    Specify test subsystem
                -tm TESTMODULE, --testmodule TESTMODULE    Specified test module
                -ts TESTSUIT, --testsuite TESTSUIT    Specify test suite
                -tc TESTCASE, --testcase TESTCASE    Specify test case
                -tl TESTLEVEL, --testlevel TESTLEVEL    Specify test level
            ```



-   See the test framework help if needed.
    -   Run the following command to query commands supported by the test platform:

        ```
        help
        ```



-   Exit the self-test platform.
    -   Run the following command to exit the test platform:

        ```
        quit
        ```



## Test Result and Log<a name="section414715805819"></a>

-   Test logs and test reports are generated after you execute the test commands.
-   Test result
    -   Reports are displayed on the console. The root path of the test result is as follows:

        ```
        reports/xxxx-xx-xx-xx-xx-xx
        ```

    -   Test case formatting result

        ```
        result/
        ```

    -   Test case log

        ```
        log/plan_log_xxxx-xx-xx-xx-xx-xx.log
        ```

    -   Report summary

        ```
        summary_report.html
        ```

    -   Test report details

        ```
        details_report.html
        ```


-   Test framework log

    ```
    reports/platform_log_xxxx-xx-xx-xx-xx-xx.log
    ```

-   Latest test reports

    ```
    reports/latest
    ```


## Repositories Involved<a name="section6299103515474"></a>

[testing subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/testing-subsystem.md)

**test\_developertest**

[test\_xdevice](https://gitee.com/openharmony/test_xdevice/blob/master/README.md)
