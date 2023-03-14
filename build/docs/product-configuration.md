# 产品配置

## 三方产品

### 配置目录

产品配置文件放置在vendor目录下，具体目录为：vendor/${company}/${product_name}/config.json。

三方厂商如果需要新加一款产品，直接在vendor/${company}目录下新增产品目录即可。

### 配置文件的格式

config.json的格式沿用轻鸿蒙的格式。

其中version是指config.json的版本号，目前固定为3.0。版本号主要是区别早期的配置格式。preloader解析产品配置时会使用到该版本号。

```
{
	"product_name": "ipcamera",                       # 产品名称
	"version": "3.0",                                 # config.json的版本号，固定为3.0
	"type": "small",                                  # 系统类型，可选["mini", "small", "standard"]
	"ohos_version": "OpenHarmony 1.0",                # 选择的OS版本
	"device_company": "hisilicon",                    # 芯片厂商
	"board": "hispark_taurus",                        # 开发板名称
	"kernel_type": "liteos_a",                        # 选择的内核类型
	"kernel_version": "3.0.0",                        # 选择的内核版本
	"subsystems": [                            
	  {
	    "subsystem": "aafwk",                         # 选择的子系统
	    "components": [
	      { "component": "ability", "features":[ "enable_ohos_appexecfwk_feature_ability = true" ] }   # 选择的组件和组件特性配置
	    ]
	  },
	  {
	   ......
	  }
	  ......
	}
}
```

## built-in产品

OpenHarmony内置的产品（比如ohos-sdk，generic-arm64）等是放置在productdefine/common/products目录下。built-in产品配置的格式是2.0。

built-in产品配置参照：https://gitee.com/openharmony/build/blob/master/docs/%E6%A0%87%E5%87%86%E7%B3%BB%E7%BB%9F%E5%A6%82%E4%BD%95%E6%B7%BB%E5%8A%A0%E4%B8%80%E4%B8%AA%E6%A8%A1%E5%9D%97.md#%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6%E8%AF%B4%E6%98%8E

