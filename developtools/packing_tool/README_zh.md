# 打包工具组件<a name="ZH-CH_TOPIC_0000001126664311"></a>

-   [简介](#section_introduction)
-   [目录](#section_directory)
-   [约束](#section_constraints)
-   [使用说明](#section_usage_instruction)
-   [相关仓](#section_related_repo)

## 简介<a name="section_introduction"></a>

本组件包含打包、拆包和包解析三个功能模块：

-   打包模块有将资源文件打包成hap包，和将多个hap包打包成app包两种模式。
-   拆包模块有从app包中拆出所有hap包，和从hap包中拆出json文件两种模式。

-   包解析模块可以根据对应解析模式，解析出指定设备类型下的hap包列表、hap包信息、签名信息等。

## 目录<a name="section_directory"></a>
```
/developtools/packing_tool
     ├── adapter
     │      ├── bundles                    # packing_tool工具的版本信息
     │      └── ohos                       # packing_tool工具实现的核心代码
     └── META-INF
            ├── collectingbininfo_tool     # bin文件转成hap文件的外部依赖配置文件
            ├── packingbin_tool            # hap文件转成bin文件的外部依赖配置文件
            └── unpacking_tool             # 解析hap文件的外部依赖配置文件
```

## 约束<a name="section_constraints"></a>
1.当打包的app包依赖hap包时，需要hap包打包执行完毕后，再执行app包的打包命令。可根据命令执行的返回值来控制。

2.如果命令中多次出现同一个命令，则取其最后一次出现的值，如--mode hap --mode app，即mode值为app。

3.如果同一命令中跟多个参数，则取距离其最近的值。如--mode hap app，mode值为hap。

4.传入的参数路径不能以“--”开头，避免与指令冲突。

5.指令必须是小写的，如：--mode，文件后缀不区分大小写。

6.建议命令中传入绝对路径。

7.打包完成后，res/下的文件会放到assets/entry/resources目录下。
assets/目录下的文件将被放入assets/目录下。
lib/下的文件将被放入libs/目录下。
maple-so/下的文件将被放入maple/目录下。
resource.index的文件将被放入assets/entry/目录下。

## 使用说明<a name="section_usage_instruction"></a>
打包hap包命令示例：
```
java -jar hmos_app_packing_tool.jar --mode <option> --json-path <option> --maple-so-path [option] --profile-path [option] --maple-so-dir [option] --dex-path [option] --lib-path [option] --resources-path [option] --shell-apk-path [option] --index-path [option] --out-path <option> --force [option]
```
参数含义及规范:

| 参数 | 是否必选 | 描述 |
|  ----  | ----  | ---- |
|--mode|是|打包类型（hap）|
|--json-path|是|.json文件路径，文件名必须为config.json。|
|--profile-path|否|CAPABILITY.profile文件路径|
|--maple-so-dir|否|maple so目录输入路径|
|--dex-path|否|1.dex文件路径，文件名必须以.dex为后缀。如果是多个dex需要用“,”分隔<br>2.dex文件路径也可以为目录|
|--lib-path|否|lib库文件路径|
|--resources-path|否|resources资源包路径|
|--shell-apk-path|否|1.shell apk文件路径，文件名必须以.apk为后缀。如果是多个apk需要用“,”分隔<br>2.shell apk文件路径也可以为目录|
|--index-path|否|.index文件路径，文件名必须为resources.index|
|--out-path|是|目标文件路径，文件名必须以.hap为后缀|
|--force|否|默认值为false，如果为true,表示当目标文件存在时，强制删除|
|--file-path|否|可指定目标文件夹，将其打入hap包内|

打包har包命令示例：
```
java -jar hmos_app_packing_tool.jar --mode <options> --json-path <option> --jar-path <option> --lib-path [option] --resources-path [option] --out-path <option> --force [option]
```
参数含义及规范:

| 参数 | 是否必选 | 描述 |
| ---- | ---- | ---- |
|--mode|是|打包类型（har）|
|--json-path|是|.json文件路径，文件名必须为config.json|
|--jar-path|否|1.jar文件路径，文件名必须以.jar为后缀。如果是多个jar需要用“，”分隔<br>2.jar文件路径也可以为目录|
|--lib-path|否|lib库文件路径|
|--res-path|否|resource资源包路径|
|--out-path|是|目标文件路径，文件名必须以.hap为后缀|
|--force|否|默认值为false，如果为true,表示当目标文件存在时，强制删除|

打包app包命令示例：
```
java -jar hmos_app_packing_tool.jar --mode <options> --hap-path <option> --out-path [option] --signature-path [option] --certificate-path [option] --pack-info-path <option> --force [option]
```
参数含义及规范:

| 参数 | 是否必选 | 描述 |
|  ----  | ----  | ---- |
|--mode|是|打包类型（app）|
|--hap-path|是|1.hap包文件路径，文件名必须以.hap为后缀。如果是多个hap包需要用“，”分隔<br>2.hap文件路径也可以是目录|
|--pack-info-path|是|文件名必须为pack.info|
|--out-path|是|目标文件路径，文件名必须以.app为后缀|
|--signature-path|否|签名路径|
|--certificate-path|否|证书路径|
|--force|否|默认值为false，如果为true,表示当目标文件存在时，强制删除|

拆hap包命令示例：
```
java -jar hmos_app_unpacking_tool.jar --mode <options> --hap-path <option> --out-path [option] --force [option] --unpackapk [option]
```
参数含义及规范:

| 参数 | 是否必选 | 描述 |
|  ----  | ----  | ---- |
|--mode|是|拆包类型（hap）|
|--hap-path|是|hap包路径|
|--out-path|是|拆包目标文件路径|
|--force|否|默认值为false，如果为true,表示当目标文件存在时，强制删除|
|--unpackapk|否|默认值为false，如果为true,表示拆包时，将apk解压出来|

拆har包命令示例：
```
java -jar hmos_app_unpacking_tool.jar --mode <options> --har-path <option> --out-path [option] --force [option]
```
参数含义及规范:

| 参数 | 是否必选 | 描述 |
|  ----  | ----  | ---- |
|--mode|是|拆包类型（har）|
|--har-path|是|har包路径|
|--out-path|是|拆包目标文件路径|
|--force|否|默认值为false，如果为true,表示当目标文件存在时，强制删除|

拆app包命令示例：
```
java -jar hmos_app_unpacking_tool.jar --mode <options> --app-path <option> --out-path [option] --force [option] --unpackapk [option]
```
参数含义及规范:

| 参数 | 是否必选 | 描述 |
|  ----  | ----  | ---- |
|--mode|是|拆包类型（app）|
|--app-path|是|app包路径|
|--out-path|是|拆包目标文件路径|
|--force|否|默认值为false，如果为true,表示当目标文件存在时，强制删除|
|--unpackapk|否|默认值为false，如果为true,表示拆包时，将apk解压出来|

## 相关仓<a name="section_related_repo"></a>

[用户程序框架子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E7%94%A8%E6%88%B7%E7%A8%8B%E5%BA%8F%E6%A1%86%E6%9E%B6%E5%AD%90%E7%B3%BB%E7%BB%9F.md)