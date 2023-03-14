# 使用说明

## 测试代码说明
> 本套测试代码，提供了3个用例

<table>
<thead>
<tr>
<th width="10%">标题</th>
<th width="15%">用例名称</th>
<th width="35%">测试步骤</th>
<th width="40%">预期结果</th>
</tr>
</thead>
<tbody>
<tr>
<td>用例1</td>
<td>接口基本功能测试</td>
<td>按默认条件编译出一个版本，刷机上电运行，检查日志</td>
<td>用例全部通过。并检查是否可搜到关键字：this is the first run of test after flash</td>
</tr>
<tr>
<td>用例2</td>
<td>数据重启可用测试</td>
<td>用例1执行完后，重启设备，检查日志</td>
<td>用例全部通过。并检查是否可搜到关键字：this is not the first run of test after flash</td>
</tr>
<tr>
<td>用例3</td>
<td>token持久化测试</td>
<td>打开 TOKEN_PERSIST_TEST 宏（去注释BUILD.gn里面第37行defines）进行编译，重新刷机上电运行，检查日志</td>
<td>用例全部通过。并检查是否可搜到关键字：this is the first run of test after flash</td>
</tr>
</tbody>
</table>


## 编译说明

### 第一步
打开当前路径BUILD.gn，修改第33行为oem_auth_config.h和oem_auth_result_storage.h 实际所在路径。  
默认为：`"//vendor/kitframework/include/"`，asr、bl等芯片平台适用。  
Hi3861芯片平台可能需要修改为：`"//vendor/hisilicon/hi3861/hals/utils/token/include"`

### 第二步
修改 `test/xts/acts/build_lite/BUILD.gn` 中 **liteos_m** 部分的代码，去掉 ActsKitFwkApiTest 所在行的注释，将其他目标都注释掉，排除测试干扰，最终大概是这样的：
```
    if (ohos_kernel_type == "liteos_m") {
      all_features += [
        # "//test/xts/acts/communication_lite/lwip_hal:ActsLwipTest",
        # "//test/xts/acts/communication_lite/softbus_hal:ActsSoftBusTest",
        # "//test/xts/acts/communication_lite/wifiservice_hal:ActsWifiServiceTest",
        # "//test/xts/acts/utils_lite/file_hal:ActsUtilsFileTest",
        # "//test/xts/acts/startup_lite/syspara_hal:ActsParameterTest",
        # "//test/xts/acts/iot_hardware_lite/iot_controller_hal:ActsWifiIotTest",
        # "//test/xts/acts/kernel_lite/kernelcmsis_hal:ActsCMSISTest",
        # "//test/xts/acts/utils_lite/kv_store_hal:ActsKvStoreTest",
        # "//test/xts/acts/security_lite/huks_hal:ActsSecurityDataTest",
        # "//test/xts/acts/hiviewdfx_lite/hilog_hal:ActsDfxFuncTest",
        # "//test/xts/acts/distributed_schedule_lite/system_ability_manager_hal:ActsSamgrTest",
        # "//test/xts/acts/update_lite/dupdate_hal:ActsUpdaterFuncTest",
        # "//test/xts/acts/startup_lite/bootstrap_hal:ActsBootstrapTest",
        "//test/xts/acts/applications/kitframework:ActsKitFwkApiTest",
      ]
```

### 第三步
按常规XTS测试的执行方式，修改当前芯片平台的链接方式，确保 **ActsKitFwkApiTest** 这个静态库会链接进最终刷机镜像里。这一步，不同芯片平台方法不同。  
如果是Hi3861，默认工程配置已添加所有XTS lib库，无需额外配置。  
如果是asr平台，需修改device/asr/asr582x/BUILD.gn：搜索`asr582x_xts_enable`，在`ldflags`里面增加一行： `"-lmodule_ActsKitFwkApiTest"`

### 第四步
请确保下面的修改已合入当前工程：   
[https://gitee.com/openharmony/xts_tools/commit/b41caca330471816a18ac24a49f955ad761b2e23](https://gitee.com/openharmony/xts_tools/commit/b41caca330471816a18ac24a49f955ad761b2e23)
[https://gitee.com/openharmony/xts_tools/commit/9b3ecb0c6f2605151bd1fe22a4a8e57b67c6e17b](https://gitee.com/openharmony/xts_tools/commit/9b3ecb0c6f2605151bd1fe22a4a8e57b67c6e17b)

### 第五步
编译，刷机，运行看日志