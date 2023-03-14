## 关于deps、external_deps的使用

在添加一个模块的时候，需要在BUILD.gn中声明它的依赖，为了便于后续处理部件间依赖关系，我们将依赖分为两种——部件内依赖deps和部件间依赖external_deps。

### 依赖分类：

**部件内依赖：** 现有模块module1属于部件part1，要添加一个属于部件part1的模块module2，module2依赖于module1，这种情况就属于部件内依赖。

**部件间依赖：** 现有模块module1属于部件part1，要添加一个模块module2，module2依赖于module1，module2属于部件part2。模块module2与模块module1分属于两个不同的部件，这种情况就属于部件间依赖。

部件内依赖使用deps，部件间依赖使用external_deps。

### 示例：

**部件内依赖示例：**

```gn
import("//build/ohos.gni")
ohos_shared_library("module1") {
  ……
  part_name = "part1"   # 必选，所属部件名称
}
```

```
import("//build/ohos.gni")
ohos_shared_library("module2") {
  ……
  deps = [
    "module1的gn target",
    ……
    ]  			        # 部件内模块依赖
  part_name = "part1"   # 必选，所属部件名称
}
```

部件内依赖和一般的依赖一样

**部件间依赖示例：**

```
import("//build/ohos.gni")
ohos_shared_library("module1") {
  ……
  part_name = "part1"   # 必选，所属部件名称
}
```

模块1所属部件的ohos.build文件

```
{
  "subsystem":"子系统名称",
  "parts": {
    "part1": {
      "inner_kits": [
        {
          "header": {
            "header_base": "头文件所属目录",	# 头文件所属目录
            "header_files": [
                "头文件名"
              ]								 # 头文件名列表
            },
            "name": "module1的gn target"
        },
      ],
      ……
    }
  }
}
```

```
import("//build/ohos.gni")
ohos_shared_library("module2") {
  ……
  external_deps = [
    "part1:module1",
    ……
  ]  			        # 部件间模块依赖，这里依赖的模块必须是依赖的部件声明在inner_kits中的模块
  part_name = "part2"   # 必选，所属部件名称
}
```

注意：部件间依赖要写在external\_deps里面，格式为”部件名:模块名"的形式，并且依赖的模块必须是依赖的部件声明在inner_kits中的模块。

