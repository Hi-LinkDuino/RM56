# hap的编译

## 术语

gn 目标： 

## hap包的构成

OpenHamony上的hap包由资源，raw assets，js assets，native库，config.json等部分构成。


## 编译系统提供的模板

编译系统提供了ohos_hap模板，用来编译hap包。

模板集成在ohos.gni中，使用之前需要引用build/ohos.gni

### ohos_hap

- 声明一个hap目标，该目标会生成一个hap包，最终将会打包到system镜像中

- 支持的变量：

  1.  hap_profile: hap包的config.json

  2. raw_assets: 原始assets，这些assets会直接拷贝到hap包的assets目录下

  3. resources: 资源文件，编译后放置在assets/entry/resources目录下

  4. js_assets: js资源，ace编译后放置在assets/js/default目录下

  5. deps: 当前目标的依赖

  6. shared_libraries: 当前目标依赖的native库

  7. hap_name: hap包的名字，可选，默认为目标名

  8. final_hap_path: 用户可以制定生成的hap的位置，可选，final_hap_path中会覆盖hap_name

  9. subsystem_name: hap包从属的子系统名，需要和ohos.build中的名字对应，否则将导致无法安装到system镜像中

  10. part_name: hap包从属的部件名，同subsystem_name

  11. js2abc: 是否需要将该hap包转换为ARK的字节码

      签名篇见：https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-app-signature.md

  12. certificate_profile: hap对应的授权文件，用于签名

  13. certificate_file: 证书文件，证书文件和授权文件，应用开发者需要去openharmony官网申请

  14. keystore_path: keystore文件，用于签名

  15. keystore_password: keystore的密码，用于签名
  
  16. key_alias: key的别名 
  
  17. module_install_name:安装时的hap包名称
  
  18. module_install_dir: 安装到system中的位置，默认安装在system/app目录下

## 一个例子

```json
import("//build/ohos.gni") # 引用ohos.gni

ohos_hap("clock") {
  hap_profile = "./src/main/config.json" # config.json
  js_assets = ["./src/main/js/default"]
  raw_assets = ["./raw_assets"]
  resources = ["./src/main/resources"]
  shared_libraries = [
    "//third_party/libpng:libpng", # native库
  ]
  certificate_profile = "../signature/systemui.p7b" # Cer文件
  hap_name = "SystemUI-NavigationBar" # 名字
  part_name = "prebuilt_hap"
  subsystem_name = "applications"
}
```

### hap解压视图

```
  Length      Date    Time    Name
---------  ---------- -----   ----
     1439  2009-01-01 00:00   assets/raw_assets                                 -----> raw_assets
      354  2009-01-01 00:00   assets/entry/resources.index                      ------> resources
        1  2009-01-01 00:00   assets/entry/resources/base/media/attributes.key  ------> resources
        1  2009-01-01 00:00   assets/entry/resources/base/media/constants.key   ------> resources
        1  2009-01-01 00:00   assets/entry/resources/base/media/contents.key    ------> resources
     6790  2009-01-01 00:00   assets/entry/resources/base/media/icon.png        ------> resources
        1  2009-01-01 00:00   assets/entry/resources/base/media/nodes.key       ------> resources
    11170  2009-01-01 00:00   assets/js/default/app.js                          ------> js_assets
       48  2009-01-01 00:00   assets/js/default/i18n/en-US.json                 ------> js_assets
       50  2009-01-01 00:00   assets/js/default/i18n/zh-CN.json                 ------> js_assets
      224  2009-01-01 00:00   assets/js/default/manifest.json                   ------> js_assets
    41481  2009-01-01 00:00   assets/js/default/pages/index/index.js            ------> js_assets
      909  2009-01-01 00:00   config.json                                       ------> hap_profile
   266248  2009-01-01 00:00   libs/libpng.z.so                                  ------> shared_libraries

```

