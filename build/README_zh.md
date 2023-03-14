# 编译构建<a name="ZH-CN_TOPIC_0000001124588563"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [约束与限制](#section2029921310472)
-   [说明](#section1312121216216)
-   [常见问题说明](#section131336181)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

编译构建子系统提供了一个基于Gn和ninja的编译构建框架。

根据产品配置，编译生成对应的镜像包。其中编译构建流程为：

1.  使用Gn配置构建目标。
2.  Gn运行后会生成ninja文件。
3.  通过运行ninja来执行编译任务。

## 目录<a name="section161941989596"></a>

```
/build                           # 编译构建主目录
├── config                       # 编译相关的配置项
├── core
│   └── gn                       # 编译入口BUILD.gn配置
├── loader                       # 各个部件配置加载、模板生成
├── ohos                         # OpenHarmony编译打包流程配置
│   ├── kits                     # kits编译打包模板和处理流程
│   ├── ndk                      # ndk模板和处理流程
│   ├── notice                   # notice模板和处理流程
│   ├── packages                 # 版本打包模板和处理流程
│   ├── sa_profile               # sa模板和处理流程
│   ├── sdk                      # sdk模板和处理流程，包括sdk中包含的模块配置
│   └── testfwk                  # 测试相关的处理
├── scripts                      # 编译相关的python脚本
├── templates                    # c/c++编译模板定义
└── toolchain                    # 编译工具链配置
```

## 约束与限制<a name="section2029921310472"></a>

-   编译环境需要Ubuntu18.04及以上版本。

-   安装编译所需的程序包。

    安装命令：

    ```
    sudo apt-get install binutils git-core gnupg flex bison gperf build-essential zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386 lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z-dev ccache libgl1-mesa-dev libxml2-utils xsltproc unzip m4
    ```


## 说明<a name="section1312121216216"></a>

1.  代码根目录下执行全量版本的编译命令：

    ```
    ./build.sh --product-name {product_name}
    ```

    编译结果镜像输出在 out/${device_name}/packages/phone/images/目录下。

2.  编译命令支持选项：

    ```
      --product-name    # 必须  编译的产品名称，如：Hi3516DV300
      --build-target    # 可选  指定编译目标，可以指定多个
      --gn-args         # 可选  gn参数，支持指定多个
      --ccache          # 可选  编译使用ccache，需要本地安装ccache
      --sparse-image    # 可选  生成sparse镜像，需要本地安装img2simg，默认镜像为raw
    ```

3.  在已有部件中增加模块。

    **支持的模板类型：**

    ```
    ohos_executable
    ohos_shared_library
    ohos_static_library
    ohos_source_set
    
    # 预编译模板：
    ohos_prebuilt_executable
    ohos_prebuilt_shared_library
    ohos_prebuilt_etc
    ```

    在模块目录下配置BUILD.gn，根据类型选择对应的模板。

    **例子：**

    _ohos\_shared\_library示例：_

    ```
    import("//build/ohos.gni")
    ohos_shared_library("helloworld") {
      sources = []
      include_dirs = []
      cflags = []
      cflags_c = []
      cflags_cc = []
      ldflags = []
      configs = []
      deps =[]  # 部件内模块依赖
    
      # 跨部件模块依赖定义，
      # 定义格式为 "部件名:模块名称"
      # 这里依赖的模块必须是依赖的部件声明在inner_kits中的模块
      external_deps = [
        "part_name:module_name",
      ]
    
      output_name = "" # 可选，模块输出名
      output_extension = "" # 可选，模块名后缀
      module_install_dir = "" # 可选，缺省在/system/lib64或/system/lib下， 模块安装路径，模块安装路径，从system/，vendor/后开始指定
      relative_install_dir = "" # 可选，模块安装相对路径，相对于/system/lib64或/system/lib；如果有module_install_dir配置时，该配置不生效
      install_images = [] # 可选，缺省值system，指定模块安装到那个分区镜像中，可以指定多个
    
      part_name = "" # 必选，所属部件名称
    }
    ```

    _ohos\_executable示例：_

    ohos\_executable模板属性和ohos\_shared\_library基本一致

    _注意：可执行模块（即ohos\_executable模板定义的）默认是不安装的，如果要安装，需要指定install\_enable = true_

    _ohos\_prebuilt\_etc示例：_

    ```
    import("//build/ohos.gni")
    ohos_prebuilt_etc("etc_file") {
      source = "file"
      deps = []  # 部件内模块依赖
      module_install_dir = "" #可选，模块安装路径，从system/，vendor/后开始指定
      relative_install_dir = "" #可选，模块安装相对路径，相对于system/etc；如果有module_install_dir配置时，该配置不生效
      install_images = [] # 可选，缺省值system，指定模块安装到那个分区镜像中，可以指定多个
      part_name = "" # 必选，所属部件名称
    }
    ```

4.  部件配置。

    部件配置在所属子系统的根目录下的 ohos.build文件中，一个子系统可以配置多个部件。

    一个部件包含module\_list、inner\_kits、system\_kits、test\_list四个部分的声明：

    -   module\_list：部件包含的模块列表
    -   inner\_kits：部件提供其它部件的接口
    -   system\_kits：部件提供给生成应用的接口
    -   test\_list：部件对应模块的测试用例

    ohos.build中各个配置，都只是声明，具体的模块配置还是在对应的BUILD.gn中。

5.  ohos-sdk编译。

    **编译命令：**

    ``` ./build.sh --product-name ohos-sdk --ccache ```

    **结果输出：**

    ``` out/sdk/packages/ohos-sdk/ ```

    **ohos-sdk模块配置：**

    ``` build/ohos/sdk/ohos_sdk_description_std.json ```



## 常见问题说明<a name="section131336181"></a>

**如何将一个模块编译并打包到版本中？**

-   模块要指定part\_name，指定它归属的部件，一个模块只能属于一个部件；
-   部件的模块，要在部件配置的module\_list中，或者可以被module\_list中的模块依赖到；
-   部件要加到对应产品的部件列表中；

[FAQ](docs/FAQ.md)

[关于deps和external_deps的使用](docs/关于deps及external_deps的使用.md)

[标准系统如何添加一个模板并参与编译](docs/标准系统如何添加一个模块.md)

[标准系统如何修改镜像文件的打包参数](docs/标准系统如何修改镜像文件的打包参数.md)

[如何跟随系统使用gn编译hap](docs/how-to-build-a-hap.md)

[开源软件Notice收集策略说明](docs/开源软件Notice收集策略说明.md)

[关于生成开源软件包](docs/生成开源软件包.md)

[构建系统编码规范与最佳实践](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/subsystems/subsys-build-gn-coding-style-and-best-practice.md)

[部件化编译最佳实践](docs/部件化编译最佳实践.md)


## 相关仓<a name="section1371113476307"></a>

编译构建子系统

**build**

