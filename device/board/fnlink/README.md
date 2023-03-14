# BES Multi-modal V200Z-R EVB
## 介绍
Multi-modal V200Z-R开发板是基于恒玄科技BES2600WM芯片，由湖南欧智通科技有限公司出品的一款高性能、多功能、高性价比AIoT SoC开发板Multi-modal V200Z-R开发板，单模组集成四核ARM处理器（最高主频1GHz），集成双频WiFi + 双模蓝牙，支持标准的802.11 a/b/g/n/协议，支持BT/BLE 5.2协议，内建多种容量的RAM（最大42MB）和Flash（最大32MB），支持MIPI DSI及CSI，适用于各种AIoT多模态VUI + GUI交互硬件场景。

开发板外观图片。

![Image text](https://gd2.alicdn.com/imgextra/i1/2839527088/O1CN01rREbi222EM27hNfHE_!!2839527088.jpg)

底板图片。

![Image text](https://gitee.com/garnetttt/img_folder/raw/26771770ea5868a8b09c6efbf511bba8f1f68c30/evb.png)
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/V200Z-R-EVB_%E7%A1%AC%E4%BB%B6%E6%A1%86%E5%9B%BE%E5%8F%8A%E5%8A%9F%E8%83%BD%E7%A4%BA%E6%84%8F_211019.png)

开发板功能框图及介绍。

![Image text](https://gitee.com/garnetttt/img_folder/raw/master/V200Z-R-EVB_%E7%A1%AC%E4%BB%B6%E6%A1%86%E5%9B%BE%E5%8F%8A%E5%8A%9F%E8%83%BD%E7%A4%BA%E6%84%8F_211019_frame.png)

## 开发板规格

|  器件类别	|  开发板|  
|  ----  | ----  | 
|  CPU	|  双核Cortex M33 Star(最高300MHz) + 双核Cortex A7（最高1GHz）|  
|  RAM	|  最大2MB SRAM + 40MB PSRAM|  
|  ROM	|  最大32MB Nor Flash支持XIP|  
|  GPIO	|  32|  
|  I2C	|  3|  
|  UART |  4|  
|  PWM	|  8|
|  SPI	|  2|  
|  Debug UART	|  支持|  
|  ADC	|  3channel 10bit|  
|  MIPI  	|  DSI 2lane 400Mbps 720P 60fps，CSI 2lane|  
|  Display Engine	|  2D graphics engine|  
|  Audio	|  支持Histreamer框架，2AMIC，|  
|  Camera	|  200万像素摄像头模组，raw data|  
|  Codec	|  内置codec|  
|  Touch Screen	|  支持|  
|  Wi-Fi	|  WiFi4 2.4G/5G双频|  
|  Bluetooth	|  BT5.2，BT classic，LE Audio，SIG mesh|  
|  Infrared	|  支持2.0|  
|  USB Host	|  支持2.0|  
|  USB Device	|  支持|  
|  TF（MicroSD）|  支持|  
|  PMIC	|  内置|  
|  thermal	|  支持温补|  
|  按键	|  Vol+/-、reset、power等|  

## 关键特性
|  组件名	|  能力介绍|  
|  ----  | ----  |
|  WLAN服务	|  提供WLAN服务能力。包括：station和hotspot（正在支持中）模式的连接、断开、状态查询等。|  
|  BLUETOOTH 服务	|  提供蓝牙 BT BLE MESH 等功能。|  
|  ASR	|  提供本地唤醒算力。|  
|  模组外设控制	|  提供操作外设的能力。包括：I2C、I2S、ADC、UART、SPI、SDIO、GPIO、PWM、FLASH等。|  
|  分布式软总线	|  在OpenHarmony分布式网络中，提供设备被发现、数据传输的能力。|  
|  设备安全绑定  	|  提供在设备互联场景中，数据在设备之间的安全流转的能力。|  
|  基础加解密	|  提供密钥管理、加解密等能力。|  
|  系统服务管理	|  系统服务管理基于面向服务的架构，提供了OpenHarmony统一化的系统服务开发框架。|  
|  启动引导	|  提供系统服务的启动入口标识。在系统服务管理启动时，调用boostrap标识的入口函数，并启动系统服务。|  
|  基础库  	|  提供公共基础库能力。包括：文件操作、KV存储管理等。|  
|  XTS	|  提供OpenHarmony生态认证测试套件的集合能力。|  
|  HDF	|  提供OpenHarmony硬件配置驱动的能力。|  
|  Kconfig	|  提供内核配置能力。|  


## 引脚定义
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/pin1.png)
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/pin2.png)
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/pin3.png)
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/pin4.png)
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/pin5.png)

## 搭建开发环境
https://gitee.com/openharmony/device_soc_bestechnic/blob/master/README.md

### 系统要求
系统要求基于Cortex-m33的liteos_m内核操作系统，采用arm-none-eabi-gcc 10.3版本toolchain,
提供16MB内存和16MB flash，或者40MB内存和32MB flash系统配置。

OpenHarmony在基于bes2600w 的芯片的Multi-modal V200Z-R模组，依赖liteos_m内核，内核依赖 cortex-m33架构配置，编译依赖arm官方的arm-none-eabi-gcc 10.3版本
OpenHarmony需要按照官方文档介绍安装环境 https://gitee.com/openharmony/device_soc_bestechnic/blob/master/README.md ，然后编译出烧录包，按照文档介绍烧录。


### 工具要求
ubuntu 18.04编译，windows10系统烧录。

1.	Ubuntu18.04系统安装
    ```
    sudo apt-get install build-essential gcc g++ make zlib* libffi-dev e2fsprogs pkg-config flex bison perl bc openssl libssl-dev libelf-dev libc6-dev-amd64 binutils binutils-dev libdwarf-dev u-boot-tools mtd-utils gcc-arm-linux-gnueabi
    ```

2.	Ubuntu18.04安装python3和pip3 
    ```
    python3 -m pip install --user ohos-build
    ```

3.	Ubuntu18.04 配置tooLchain [arm-none-eabi-gcc](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2):

    下载[arm-none-eabi-gcc 编译工具](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2)
    解压 [gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2) 安装包至\~/toolchain/路径下。

       ```shell
       mkdir -p ~/toolchain/
       tar -zxvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C ~/toolchain/
       ```
    设置环境变量。
    
       ```
       vim ~/.bashrc
       ```
    
       将以下命令拷贝到.bashrc文件的最后一行，保存并退出。
    
       ```
       export PATH=~/toolchain/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH
       ```
    生效环境变量。
    
       ```
       source ~/.bashrc
       ```

4.	Win 10安装串口驱动CP2102usbqd 

### 搭建过程

https://gitee.com/openharmony/device_soc_bestechnic/blob/master/README.md

## 编译调试

https://gitee.com/openharmony/device_soc_bestechnic/blob/master/README.md

按下开发板上reset power按钮，通过串口调试工具打印串口log。

## 首个示例

代码默认有一个智能面板的示例。

## 参考资源

https://gitee.com/openharmony/device_soc_bestechnic/blob/master/README.md

https://item.taobao.com/item.htm?spm=a1z10.3-c-s.w4002-16847573468.28.35937883ZrIoGt&id=660443865533

## 联系
![Image text](https://gitee.com/garnetttt/img_folder/raw/master/qun.png)



**********