# 产品形态配置<a name="ZH-CN_TOPIC_0000001079317008"></a>

-   [简介](#section11660541593)
-   [配置仓目录结构](#section113275517516)
-   [配置示例](#section178021418115315)
-   [常见问题说明](#section174312714582)
-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

产品形态配置仓。

主要包括产品所属的系统类型，产品名称，对应的device配置，产品的部件列表等。

支持的产品需要在productdefine/common/products/ 目录下有同名的配置文件，编译时找不到会报错。在productdefine/common/device/ 目录下要有产品对应的device的同名配置文件。

产品配置文件中，要指定产品所属的系统类型，使用的device，包含的部件列表等。

## 配置仓目录结构<a name="section113275517516"></a>

```
productdefine/common
├── base                         # 不同系统类型的最小系统，开发者不用配置，系统提供
│   └── standard_system.json
├── device                       # device配置，配置文件按照device name命名
│   └── hi3516dv300.json
└── products                     # 产品配置，配置文件按照product name命名
    └── Hi3516DV300.json
```

## 配置示例<a name="section178021418115315"></a>

**产品配置示例：** Hi3516DV300.json

```
{
  "version": "2.0",                  # 配置文件格式版本
  "product_name": "Hi3516DV300",     # 产品名
  "product_company": "hisilicon",    # 产品厂商名
  "product_device": "hi3516dv300",   # 产品对应的device名，会读取device下的同名配置
  "type": "standard",                # 系统类型，读取base下对应的系统配置
  "inherit": [],                     # 继承的部件列表配置, 可以继承通用的部件列表
  "product_build_path": "",          # product对应的子系统路径
  "parts":{                          # 产品的部件列表
    "common:common":{},
    ......
  }
}
```

每个产品都要配置对应的系统类型，当前仅支持配置："standard",，产品会继承对应的系统类型的基础部件列表，从productdefine/common/base/ 目录下读取。

product_build_path是product对应子系统路径，编译时会扫描对应目录下的部件配置。

**device配置示例：** hi3516dv300.json

需要提供一个产品配置中product\_device同名的配置文件，配置device信息和board信息。

```
{           
  "device_name": "hi3516dv300",   # device名
  "device_company": "hisilicon",  # device厂商名
  "target_os": "ohos",            # 在设备上部署的操作系统名
  "target_cpu": "arm",            # 设备的cpu类型
  "kernel_version": "",
  "device_build_path": ""         # device对应的子系统路径
}
```

device_build_path是device对应子系统路径，编译时会扫描对应目录下的部件配置。

**产品部件类表的生成：**

1.  先加载对应系统类型的最小系统部件列表；
2.  加载产品对应的device配置的部件列表；
3.  加载产品继承的通用部件列表；
4.  加载产品配置的部件列表；
5.  一个产品包含的所有部件，从上面几个配置中来，同名的后面的会覆盖前面的。

## 常见问题说明<a name="section174312714582"></a>

**如何给对应的产品新加一个部件？**

在productdefine/common/products/ 目录下，找到对应的产品同名的配置文件，在parts中添加部件信息，格式为："子系统名:部件名":\{\}。

**如何查看一个指定产品的最终的部件列表？**

在编译启动后，编译系统会解析配置文件，生成产品的全部的部件列表，输出为：out/build\_configs/\{产品名\}/preloader/parts.json。

## 相关仓<a name="section1371113476307"></a>

productdefine/common

