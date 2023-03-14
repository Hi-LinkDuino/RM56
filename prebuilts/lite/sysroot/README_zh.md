# sysroot<a name="ZH-CN_TOPIC_0000001096759155"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [约束](#section119744591305)
-   [编译构建](#section137768191623)
-   [使用说明](#section68313135353)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

sysroot是一个用作clang编译器查找标准库和头文件的根目录，其中libc库是由开源库musl编译得到。

## 目录<a name="section161941989596"></a>

```
/prebuilts/lite/sysroot
├── build                   # 工具链构建目录，包括构建脚本
├── usr                     # 对外C库及头文件
│   ├── include             # 对外头文件存放目录
│   │  └── arm-liteos       # 工具链对应的芯片架构
│   └── lib                 # 对外C库存放目录
│       └── arm-liteos      # 工具链对应的芯片架构
```

## 约束<a name="section119744591305"></a>

只适用于OpenHarmony内核。

## 编译构建<a name="section137768191623"></a>

musl库bug修复或者新版本更新时，需要重新编译构建新的libc库。在build目录下执行`make`即可编译构建出新的libc库，存放于/prebuilts/lite/sysroot/usr目录下。

## 使用说明<a name="section68313135353"></a>

-   将clang编译器bin目录设置到环境变量中，编译环境的搭建可以参考[快速入门](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/Readme-CN.md)。
-   编译用户程序helloworld.c示例如下：

```
clang -o helloworld helloworld.c -target arm-liteos --sysroot=/my_ohos_root_path/prebuilts/lite/sysroot/
```

## 相关仓<a name="section1371113476307"></a>

[内核子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%86%85%E6%A0%B8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[prebuilts\_lite\_sysroot](https://gitee.com/openharmony/prebuilts_lite_sysroot/blob/master/README_zh.md)

