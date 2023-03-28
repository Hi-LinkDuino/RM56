

### 环境&编译：Ubuntu

ubuntu 16.04/18.04

1. 下载 [gcc-arm-bi-9-2019-q4-major-x86_64-linux.tar.bz2](https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-bi-9-2019-q4-major-x86_64-linux.tar.bz2) ，解压至~/toolchain/

   ```
   mkdir -p ~/toolchain/
   tar -xvf gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 -C ~/toolchain/
   ```

2. 设置环境变量：.bashrc文件添加

   ```bash
   export PATH=~/toolchain/gcc-arm-none-eabi-9-2019-q4-major/bin:$PATH
   ```

3. 生效环境变量：`source ~/.bashrc`

4. 编译

   ```bash
   tools/build.sh
   #输出：out/best2600w_aiot/best2600w_aiot.bin
   #可带参数编译：
   FLASH_TYPE=<n>
   # 0 - 16M norflash with 8M psram
   # 1 - 32M norflash with 32M psram
   # 2 - 32M norflash with 8M psram for cmcp running (default)
   DEBUG=<n>
   # 0 - release version
   # 1 - debug version (default)
   ```
   
   

### tools介绍

```bash
tools/
├─── burn/
|    ├─── dldtool.exe  # windows下download命令行程序，不要直接运行
|    ├─── dldtool32    # 32位linux下download命令行程序，不要直接运行
|    ├─── dldtool64    # 64位linux下download命令行程序，不要直接运行
|    ├─── factory.bin  # 工厂区预置数据包
|    ├─── programmer_16M.bin # 16M norflash ramrun, linux&windows通用
|    └─── programmer_32M.bin # 32M norflash ramrun, linux&windows通用
|    
├─── write_flash_gui_aiot_16M/	# windows下16M norflash下载工具
|    ├─── cpbin.bat  #固件从out目录拷贝到ota_bin\best2600w_aiot.bin
|    ├─── wifi_Download_cfg.yaml  #下载配置文件，按需修改
|    └─── Wifi_download_main.exe  #下载工具执行文件
|
├─── write_flash_gui_aiot_32M/  # windows下32M norflash下载工具
|
├─── audiotool_v3.5.zip  #音频工具，解压在windows运行，doc目录有文档
├─── build.sh  #swift项目编译脚本，使用方法如上所示
├─── burn.sh   #linux下download脚本，可直接使用
├─── burn-win_16M.bat #win下download脚本，适用于安装虚拟机隐射网盘到win，修改端口号即可直接运行
├─── burn-win_32M.bat #同上，32M版本
├─── genasm.sh  #linux下反汇编脚本，Usage: tools/genasm.sh [a|m]
├─── lzma       #压缩，内部使用，无须关注
└─── mklittlefs #打包预置文件系统数据包，内部使用，无须关注
```

下载工具在windows下运行，另须安装usb转串口驱动



> 注：audiotool暂时只能**<u>脱机调试</u>**音频，保存参数，同步到代码编译，**<u>联机调试</u>**需要专用的固件，后续再提供



### Wifi & BT 测试

详见文档

```bash
doc/BES2600WM_WiFi BT test flow-version2.6_20220112.pdf
```



**<u>*待更新。。。*</u>**