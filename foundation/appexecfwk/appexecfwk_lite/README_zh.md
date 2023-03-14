# 包管理组件<a name="ZH-CN_TOPIC_0000001061838370"></a>

-   [简介](#section11660541593)
-   [目录](#section1464106163817)
-   [相关仓](#section93061357133720)

## 简介<a name="section11660541593"></a>

**包管理组件**，是OpenHarmony为开发者提供的安装包管理框架。包管理组件的由如下模块组成：

**图 1**  包管理组件框架图<a name="fig1047932418305"></a>  
![](figures/包管理组件框架图.png "包管理组件框架图")

-   **BundleKit**：是包管理服务对外提供的接口，有安装/卸载接口、包信息查询接口、包状态变化监听接口。
-   **包扫描子模块**：用来解析本地预制或者安装的安装包，提取里面的各种信息，供管理子模块进行管理，持久化。

-   **包安装子模块**：负责安装，卸载，升级一个包。
-   **包安装服务**：一个单独进程的用于创建删除安装目录，具有较高的权限。

-   **包管理子模块**：管理安装包相关的信息，存储持久化包信息。

-   **包安全管理子模块**：签名检查、权限授予、权限管理。

## 目录<a name="section1464106163817"></a>

```
/foundation/appexecfwk/appexecfwk_lite
     ├── frameworks
     │      └── bundle_lite                # 管理BundleKit与包管理服务通信的客户端代码
     ├── interfaces
     │      ├── kits
     │      │     └── bundle_lite         # BundleKit为开发者提供的接口
     │      └── innerkits
     │             └── bundlemgr_lite      # BundleKit实现的核心代码，及包管理服务为其它子系统提供的接口
     ├── services
     │      └── bundlemgr_lite             # 包管理服务的实现代码
     └── utils
             └── bundle_lite                # 包管理服务实现中用到的工具性的代码
```

-   包管理服务为BundleMs，服务运行于foudation进程中；
-   BundleMs注册到sa\_manager中，sa\_manager运行于foundation进程中，sa\_manager为BundleMs创建线程运行环境。具体创建BundleMs服务的方式以及使用该服务的方式，可参考系统服务框架子系统；
-   系统启动后，BundleMs会随系统启动而启动；
-   使用bm工具可以安装指定的hap包（以hispark\_taurus为例，bm工具在系统构建后放置在out/hispark\_taurus/ipcamera\_hispark\_taurus/dev\_tools/bin下）：

```
./bin/bm install -p /nfs/xxxx.hap
```

## 相关仓<a name="section93061357133720"></a>

[用户程序框架子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E7%94%A8%E6%88%B7%E7%A8%8B%E5%BA%8F%E6%A1%86%E6%9E%B6%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[aafwk\_aafwk\_lite](https://gitee.com/openharmony/aafwk_aafwk_lite/blob/master/README_zh.md)

**appexecfwk\_appexecfwk\_lite**

