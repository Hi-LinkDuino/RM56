# 资源管理组件介绍<a name="ZH-CN_TOPIC_0000001073289170"></a>

-   [简介](#section11660541593)
-   [目录](#section1844145112017)
-   [说明](#section17235135618274)
-   [约束](#section568761318105)
-   [相关仓](#section15583142420413)

## 简介<a name="section11660541593"></a>

资源管理组件为应用提供加载多语言界面资源的能力，如不同语言下提供不同的应用名称或图标等。

## 目录<a name="section1844145112017"></a>

资源管理组件源代码目录结构如下所示：

```
/base/global/
├── resmgr_lite               # 资源管理代码仓
│   ├──  frameworks           # 资源管理核心代码
│   │   ├── resmgr_lite       # 资源解析核心代码
│   │   │   ├── include       # 资源管理头文件
│   │   │   ├── src           # 资源管理实现代码
│   │   │   └── test          # 资源管理测试代码
│   ├──  interfaces           # 资源管理接口
│   │   └── innerkits         # 资源管理对子系统间接口
```

## 说明<a name="section17235135618274"></a>

调用GLOBAL\_GetValueById接口获取应用的资源信息 。

```
#include <stdint.h>
#include <stdio.h>
#include "global.h"

uint32_t id = 0x16777216;
char *values = NULL;
int32_t re = GLOBAL_GetValueById(id, "/system/data/resources.index", &values); // 调用指定应用下某一条资源，并写入values中
std::count << values << std::endl;
if (values != NULL) {
    free(values);
}
```

## 约束<a name="section568761318105"></a>

**语言限制**：C/C++语言

## 相关仓<a name="section15583142420413"></a>

[全球化子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%85%A8%E7%90%83%E5%8C%96%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

**global\_resmgr\_lite**

[global\_i18n\_lite](https://gitee.com/openharmony/global_i18n_lite/blob/master/README_zh.md)

