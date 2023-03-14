# xdevice组件<a name="ZH-CN_TOPIC_0000001083129731"></a>

-   [简介](#section15701932113019)
-   [目录](#section1791423143211)
-   [约束](#section118067583303)
-   [使用](#section2036431583)
-   [相关仓](#section260848241)

## 简介<a name="section15701932113019"></a>

xdevice是OpenHarmony中为测试框架的核心组件，提供用例执行所依赖的相关服务。

xdevice主要包括以下几个主要模块：

-   command，用户与测试平台命令行交互模块，提供用户输入命令解析，命令处理。
-   config，测试框架配置模块，提供测试平台串口连接方式和USB连接方式的不同配置选项。
-   driver，测试用例执行器，提供测试用例分发，执行，结果收集等主要测试步骤定义。
-   report，测试报告模块，提供测试结果解析和测试报告生成。
-   scheduler，测试框架调度模块，提供不同类型的测试执行器调度的调度功能。
-   environment，测试框架的环境配置模块，提供设备发现，设备管理的功能。
-   testkit，测试框架工具模块，提供json解析，网络文件挂载等操作。
-   resource，测试框架资源模块，提供设备连接配置文件和报告模板定义。

## 目录<a name="section1791423143211"></a>

```
xdevice
├── config                    # xdevice组件配置
│     ├── user_config.xml    # xdevice环境配置
├── resource                  # xdevice组件资源
│     ├── tools              # 版本烧录工具
├── src                       # 组件源码目录
│     ├── xdevice
├── extension                 # xdevice扩展模块
│     ├── src                # 扩展模块源码
│     └── setup.py           # xdevice扩展模块安装脚本
```

## 约束<a name="section118067583303"></a>

运行环境要求：

-   python版本\>=3.7.5
-   pyserial\>=3.3
-   paramiko\>=2.7.1
-   rsa\>=4.0

## 使用<a name="section2036431583"></a>

-   **安装xdevice**
    1.  打开xdevice安装目录。
    2.  打开控制台，执行如下命令：

        ```
        python setup.py install
        ```


-   **安装extension**
    1.  打开extension安装目录。
    2.  打开控制台，执行如下命令：

        ```
        python setup.py install
        ```


-   **修改user\_config.xml**

    user\_config.xml是框架提供的用户配置文件，用户可以根据自身环境信息配置相关内容，具体介绍如下：

    **1、environment环境相关配置：**

    -   设备类型一

        >![](figures/icon-note.gif) **说明：** 
        >ip/port: 表示远程设备地址，默认情况为空，表示使用本地设备，ip地址为127.0.0.1，port为本机hdc启动端口号；
        >​sn: 过滤执行测试设备，若设置为SN1，则表示只有设备SN1能够支持后续run命令执行，其他设备分配状态设置为Ignored，不参与命令执行，可通过list devices命令中Allocation字段来查看sn设置，可配置多个sn，中间以;隔开；

    -   设备类型二

        >![](figures/icon-note.gif) **说明：** 
        >type: 设备连接方式，com表示连接方式是串口
        >label: 表示设备种类，如wifiiot
        >serial: 表示一个串口定义
        >serial/com 表示本地连接的串口，如COM20 serial/type 表示串口类型，cmd是命令串口，deploy是刷机串口，社区版本cmd和deploy使用同一个串口，com值相同
        >serial/baud\_rate、data\_bits、stop\_bits、timeout: 为串口波特率等串口参数 ，一般采用默认值即可。


    **2、测试用例目录设置**

    dir: 指定测试用例目录。

    **3、nfs挂载**

    >![](figures/icon-note.gif) **说明：** 
    >server: nfs挂载配置，label取值为NfsServer。
    >server/ip: 挂载环境IP地址。
    >server/port: 挂载环境端口。
    >server/username: 登录用户名。
    >server/password: 登录用户密码。
    >server/dir: 对应挂载的外部路径。
    >server/remote: nfs服务器与xDevice执行机不在同一台机器时，remote配置为true，否则为false。

-   **选定任务类型**
-   **启动框架**
-   **执行指令**

    框架指令可以分为三组：help、list、run。在指令序列中，以run为最常用的执行指令。

    **1、help**

    输入help指令可以查询框架指令帮助信息。

    ```
    help:
         use help to get information.  
    usage:
         run:  Display a list of supported run command.
         list: Display a list of supported device and task record.  
    Examples:
         help run
         help list
    ```

    >![](figures/icon-note.gif) **说明：** 
    >help run：展示run指令相关说明
    >help list：展示 list指令相关说明

    **2、list**

    list指令用来展示设备和相关的任务信息

    ```
    list:
         This command is used to display device list and task record.  
    usage:
          list
          list history
          list <id>  
    Introduction:
         list:         display device list
         list history: display history record of a serial of tasks
         list <id>:    display history record about task what contains specific id  
    Examples:
         list
         list history
         list 6e****90
    ```

    >![](figures/icon-note.gif) **说明：** 
    >list: 展示设备信息
    >list history: 展示任务历史信息
    >list <id\>: 展示特定id的任务其历史信息

    **3、run**

    run指令主要用于执行测试任务

    ```
    run:
         This command is used to execute the selected testcases.
         It includes a series of processes such as use case compilation, execution, and result collection.  
    usage: run [-l TESTLIST [TESTLIST ...] | -tf TESTFILE
                [TESTFILE ...]] [-tc TESTCASE] [-c CONFIG] [-sn DEVICE_SN]
                [-rp REPORT_PATH [REPORT_PATH ...]]
                [-respath RESOURCE_PATH [RESOURCE_PATH ...]]
                [-tcpath TESTCASES_PATH [TESTCASES_PATH ...]]
                [-ta TESTARGS [TESTARGS ...]] [-pt]
                [-env TEST_ENVIRONMENT [TEST_ENVIRONMENT ...]]
                [-e EXECTYPE] [-t [TESTTYPE [TESTTYPE ...]]]
                [-td TESTDRIVER] [-tl TESTLEVEL] [-bv BUILD_VARIANT]
                [-cov COVERAGE] [--retry RETRY] [--session SESSION]
                [--dryrun] [--reboot-per-module] [--check-device]
                [--repeat REPEAT]
                action task  
    Specify tests to run.
      positional arguments:
       action                Specify action
       task                  Specify task name,such as "ssts", "acts", "hits"
    ```

    >![](figures/icon-note.gif) **说明：** 
    >一个基本的run指令结构如下：
    >```
    >run [task name] -l module1;moudle2
    >```
    >task name：任务类型。一般为ssts、acts、hits。非必选项
    >-l :指定执行测试用例，多个测试用例，中间用；隔开
    >module：被测试的模块。一般在testcases目录下存在对应的\\.json文件
    >此外，其他参数可以作为约束条件，附加到这个基本指令之上使用。常用的如:
    >-sn: 过滤执行测试设备，若设置为SN1，则表示只有设备SN1执行用例
    >-c: 重新指定user\_config.xml。
    >-rp: 报告生成路径。默认为xxx/xdevice/reports目录。指定目录后，优先级:指定目录\>xxx/xdevice/reports目录。
    >-tcpath：环境目录，默认为xxx/xdevice/testcases目录。指定目录后，优先级:指定目录\>xxx/xdevice/testcases目录
    >-respath：测试套目录，默认为xxx/xdevice/resource目录。指定目录后，优先级:指定目录\>xxx/xdevice/resource目录
    >--reboot-per-module: 执行前先重启设备

-   **查看执行结果**

    框架执行run指令，控制台会输出对应的log打印，还会生成对应的执行结果报告。如果使用了-rp参数指定报告路径，那么报告就会生成在指定的路径下。否则报告会存放在默认目录。

    ```
    当前报告目录（默认目录/指定目录）
         ├── result（模块执行结果存放目录）
         │     ├── <模块名>.xml
         │     ├──  ... ... 
         │      
         ├── log (设备和任务运行log存放目录)
         │     ├── <设备1>.log
         │     ├── ... ...
         │     ├── <任务>.log
         ├── summary_report.html（测试任务可视化报告）
         ├── summary_report.html（测试任务数据化报告）
         └── ... ...
    ```


## 相关仓<a name="section260848241"></a>

[测试子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E6%B5%8B%E8%AF%95%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**test\_xdevice**

[test\_developertest](https://gitee.com/openharmony/test_developertest/blob/master/README_zh.md)
