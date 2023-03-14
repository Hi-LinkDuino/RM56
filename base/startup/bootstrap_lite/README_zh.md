# bootstrap启动引导组件<a name="ZH-CN_TOPIC_0000001082300996"></a>

-   [简介](#section469617221261)
-   [目录](#section1464106163817)
-   [约束](#section12212842173518)
-   [使用说明](#section1483211215513)
-   [相关仓](#section641143415335)

## 简介<a name="section469617221261"></a>

bootstrap启动引导组件，提供了各服务和功能的启动入口标识。在SAMGR启动时，会调用boostrap标识的入口函数，并启动系统服务。

## 目录<a name="section1464106163817"></a>

```
base/startup/bootstrap_lite/    # 启动引导组件
└── services
    └── source               # 启动引导组件源文件目录
```

## 约束<a name="section12212842173518"></a>

目前支持轻量系统设备（参考内存≥128KB），如Hi3861V100。

## 使用说明<a name="section1483211215513"></a>

bootstrap组件无需单独配置，在SAMGR启动时会自动调用，用于启动系统服务。

## 相关仓<a name="section641143415335"></a>

[启动恢复子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%90%AF%E5%8A%A8%E6%81%A2%E5%A4%8D%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[startup\_syspara\_lite](https://gitee.com/openharmony/startup_syspara_lite/blob/master/README_zh.md)

[startup\_appspawn\_lite](https://gitee.com/openharmony/startup_appspawn_lite/blob/master/README_zh.md)

**[startup\_bootstrap\_lite](https://gitee.com/openharmony/startup_bootstrap_lite/blob/master/README_zh.md)**

[startup\_init\_lite](https://gitee.com/openharmony/startup_init_lite/blob/master/README_zh.md)

