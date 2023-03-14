# syspara系统属性组件<a name="ZH-CN_TOPIC_0000001081867232"></a>

-   [简介](#section469617221261)
-   [目录](#section692981610397)
-   [约束](#section741841418125)
-   [使用说明](#section1464106163817)
-   [相关仓](#section641143415335)

## 简介<a name="section469617221261"></a>

系统属性组件，根据OpenHarmony产品兼容性规范提供获取设备信息的接口，如：产品名、品牌名、厂家名等，同时提供设置/读取系统属性的接口。

## 目录<a name="section692981610397"></a>

```
base/startup/syspara_lite/    # 系统属性组件
├── frameworks             # 系统属性组件源文件目录
├── hals                   # 系统属性组件硬件抽象层头文件目录
└── interfaces             # 系统属性组件对外接口目录
```

## 约束<a name="section741841418125"></a>

-   使用C语言开发。
-   同时支持轻量系统设备（参考内存≥128KB），如Hi3861V100以及小型系统设备（参考内存≥1MB），如Hi3516DV300、Hi3518EV300。
-   系统属性各字段由OEM厂商负责定义，当前方案仅提供框架及默认值。具体值需产品方按需进行调整。

## 使用说明<a name="section1464106163817"></a>

获取系统属性

```
char* value1 = GetDeviceType();
printf("Device type =%s\n", value1);
free(value1);
char* value2 = GetManufacture();
printf("Manufacture =%s\n", value2);
free(value2);
char* value3 = GetBrand();
printf("GetBrand =%s\n", value3);
free(value3);
```

修改系统版本号

```
系统版本号参数定义在startup_init_lite仓库的/services/etc/param/ohos.para文件
const.product.software.version参数的值即为展示的系统版本号
等同于之前修改宏OHOS_DISPLAY_VERSION[]的值
```

## 相关仓<a name="section641143415335"></a>

[启动恢复子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%90%AF%E5%8A%A8%E6%81%A2%E5%A4%8D%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**[startup\_syspara\_lite](https://gitee.com/openharmony/startup_syspara_lite/blob/master/README_zh.md)**

[startup\_appspawn\_lite](https://gitee.com/openharmony/startup_appspawn_lite/blob/master/README_zh.md)

[startup\_bootstrap\_lite](https://gitee.com/openharmony/startup_bootstrap_lite/blob/master/README_zh.md)

[startup\_init\_lite](https://gitee.com/openharmony/startup_init_lite/blob/master/README_zh.md)

