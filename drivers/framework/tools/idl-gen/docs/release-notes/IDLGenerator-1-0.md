# IDLGenerator 1.0

- [版本概述](#版本概述)
- [遗留问题](#遗留问题)

## 版本概述
首次发布，支持以下特性：
- 支持通过-f参数指定头文件所在路径，路径支持绝对路径和相对路径
- 支持一次转换多个.h头文件
- 支持.h文件在不同目录下
- 自动解析.h文件中包含的头文件并转换成.idl
- 类型定义统一定义到每级目录的Types.idl中

## 遗留问题
- 枚举类型中非10进制、负数和位运算不支持转换，举例：
```
typedef enum {
    DISPLAY_START = 1 //支持  
    DISPLAY_SUCCESS = 0x1234u, //不支持
    DISPLAY_FAILURE = -1, //不支持
    HBM_USE_CPU_READ = (1 << 0), //不支持
} DispErrCode;
```
- 只支持结构体类型的callback对象生成idl，不支持函数指针类型的callback对象生成idl，举例:
```
IFoo.h
struct IFooCallback {
    int32_t (*PushData)(struct IFooCallback *self, const char* message);
};
struct IFoo {
    int32_t (*SendCallbackObj)(struct IFoo *self, struct IFooCallback* cbObj //支持);
};
```
```
IFoo.h
typedef int32_t (*IFooCallback)(const char* message);
struct IFoo {
    int32_t (*SendCallbackObj)(struct IFoo *self, IFooCallback* cbObj //不支持);
};
```