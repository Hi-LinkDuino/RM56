# Get Started

[toc]

## 开发环境搭建

系统要求： Ubuntu18.04 64位系统版本

1. 安装依赖库和工具
2. 安装Python3
3. 安装hb
4. 安装arm-none-eabi-gcc

请参考：[README.md · OpenHarmony/device_soc_bestechnic - Gitee.com](https://gitee.com/openharmony/device_soc_bestechnic/blob/master/README.md#安装的库和工具)



## 编译

> 如果没有安装gn/ninja，第一次编译需要运行`build/prebuilts_download.sh`

```bash
hb set
#选择bestechnic>2003_dispay_demo
#只需执行一次，除非ohos_config.json被删除，或需要切换项目
hb build #编译
```

编译失败，可查看`out\v200zr\2003_display_demo\build.log`

编译成功，则输出到`dist\`目录

```bash
write_flash_gui_2003_display_demo_xxxxx.tar.gz   #GUI可视化烧录工具
dist_2003_display_demo_xxxxx.tar.gz              #命令行烧录包
burn\                                            #命令行烧录包
```



## 烧录

GUI烧录工具会一次性烧录全部代码+文件系统+factory数据，耗时会有点长

开发我们一般都是用命令行烧录工具，运行`dist\burn\burn.bat`出现如下提示

```bash
===================================================
Please input the following option for programing
===================================================
  0. Quit
  1. rtos
  2. littlefs(norflash filesystem)
  3. factory
  4. boot1
  5. boot2
  6. all images(littlefs rtos sensor)
===================================================
Please input option(default 1):
```

任选其一，等待烧录，单击 开发板reset 或者 power 进行烧录

注1：第一次烧录6

注2：日常开发，只需选择1烧录rtos即可

注3：如果仅仅是更新文件系统，烧录2即可



## 其它

### 编译开关配置

编译开关在`vendor/bestechnic/2003_display_demo/config.json`里添加

```json
{
  "product_name": "2003_display_demo",
  "cflags": [
    "-DTEST_FLAG=1"
  ],
}
```



### 更新文件系统

`hb build`时会将`vendor/bestechnic/2003_display_demo/fs/data/`打包成文件系统镜像littlefs.bin

```bash
data/          
├─dream_it_possible.mp3               # histreamer player测试文件
├─panel/                              # js app
└─font.ttf                            # 字库文件
```

如果仅仅是更新js app，直接替换`data/panel`即可，然后`hb build`生成littlefs.bin

