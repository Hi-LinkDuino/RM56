# Get Started

[toc]

## 开发环境搭建

系统要求： Ubuntu18.04 64位系统版本

1. 安装依赖库和工具
2. 安装Python3
3. 安装hb
4. 安装arm-none-eabi-gcc



### 安装依赖库和工具

```bash
sudo apt-get install build-essential gcc g++ make zlib* libffi-dev e2fsprogs pkg-config flex bison perl bc openssl libssl-dev libelf-dev libc6-dev-amd64 binutils binutils-dev libdwarf-dev u-boot-tools mtd-utils gcc-arm-linux-gnueabi
```



### 安装Python3.8

运行`python3 --version`查看版本，如果不是3.8，则须安装

```bash
sudo apt-get install python3.8
#设置python3软链接为python3.8
sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 1
```

安装并升级Python包管理工具（pip3），任选如下一种方式。

- **命令行方式：**

  ```
  sudo apt-get install python3-setuptools python3-pip -y
  sudo pip3 install --upgrade pip
  ```

- **安装包方式：**

  ```
  curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
  python get-pip.py
  ```



### 安装hb

```bash
#如果安装了hb,须先卸载
pip3 uninstall ohos-build 
#进入源码目录安装
pip3 install build/lite
```

hb默认安装到`~/.local/bin/`目录下，需要将该路径设置到PATH中，修改`~/.bashrc`

```bash
vi ~/.bashrc
```

将以下命令拷贝到.bashrc文件的最后一行，保存并退出。

```bash
export PATH=~/.local/bin:$PATH
```

执行如下命令更新环境变量。

```bash
source ~/.bashrc
```



### 安装arm-none-eabi-gcc

1. 下载： [gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2) 

2. 解压安装包至~/toolchain/路径下。

   ```bash
   mkdir -p ~/toolchain/
   tar -jxvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C ~/toolchain/
   ```

3. 设置环境变量。

   ```
   vim ~/.bashrc
   ```

   将以下命令拷贝到.bashrc文件的最后一行，保存并退出。

   ```bash
   export PATH=~/toolchain/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH
   ```

4. 生效环境变量。

   ```bash
   source ~/.bashrc
   ```



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



### 编译常见问题

#### Q1：hb_error: Please call hb utilities inside source root directory

原因：hb命令必须在源码目录运行，可以在源码目录的任意一个子目录下运行



#### Q2：编译报错：gn、ninja找不到

```bash
#在源码根目录下运行
build/prebuilts_download.sh
```



#### Q3：ubuntu20.04编译报错

```bash
Command: /usr/bin/python3 build/loader/load.py --subsystem-config-file out/preloader/2003_display_demo/subsystem_config.json --platforms-config-file out/preloader/2003_display_demo/platforms.build --source-root-dir xxxx/ --gn-root-out-dir out/v200zr/2003_display_demo --target-os ohos --target-cpu arm --os-level mini --ignore-api-check xts common developertest
Returned 1.
stderr:

Traceback (most recent call last):
  File "build/loader/load.py", line 23, in <module>
    from loader import generate_targets_gn  # noqa: E402
  File "build/loader/generate_targets_gn.py", line 25, in <module>
    from jinja2 import Template  # noqa: E402
  File "third_party/jinja2/__init__.py", line 12, in <module>
    from .environment import Environment
  File "third_party/jinja2/environment.py", line 25, in <module>
    from .defaults import BLOCK_END_STRING
  File "third_party/jinja2/defaults.py", line 3, in <module>
    from .filters import FILTERS as DEFAULT_FILTERS  # noqa: F401
  File "third_party/jinja2/filters.py", line 13, in <module>
    from markupsafe import soft_unicode
ImportError: cannot import name 'soft_unicode' from 'markupsafe' (~/.local/lib/python3.8/site-packages/markupsafe/__init__.py)

file: out/v200zr/2003_display_demo/.ninja_log not exists
parse file fail
```

**解决方法：**

安装markupsafe 2.0.1版本

```bash
python3 -m pip install markupsafe==2.0.1
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

