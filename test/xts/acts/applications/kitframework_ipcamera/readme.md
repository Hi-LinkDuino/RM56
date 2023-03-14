# 使用说明

## 测试用例说明
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
<td>按默认条件编译出一个测试bin文件，放入设备运行，检查日志</td>
<td>用例全部通过。并检查是否可搜到关键字：this is the first run of test after flash</td>
</tr>
<tr>
<td>用例2</td>
<td>数据重启可用测试</td>
<td>用例1执行完后，重启设备，再次运行测试bin文件，检查日志</td>
<td>用例全部通过。并检查是否可搜到关键字：this is not the first run of test after flash</td>
</tr>
<tr>
<td>用例3</td>
<td>token持久化测试</td>
<td>
1、将设备恢复出厂<br/>
2、打开 TOKEN_PERSIST_TEST 宏（去注释BUILD.gn里面倒数第二行defines）进行编译，将新生成的测试bin文件放入设备，运行，检查日志
</td>
<td>用例全部通过。并检查是否可搜到关键字：this is the first run of test after flash</td>
</tr>
</tbody>
</table>


## 编译说明

### 第一步
当前路径BUILD.gn中修改include_dirs最后一行为oem_auth_config.h和oem_auth_result_storage.h 实际所在路径。 
默认为："//vendor/kitframework/include/"  
例如君正x2000的oem_auth_config.h和oem_auth_result_storage.h可能要修改为："//vendor/ingenic/smartpen/adapter/kitframework/include"

### 第二步
BUILD.gn中修改deps最后两行为oem_auth_config.a和oem_auth_result_storage.a实际所在路径。  
例如君正x2000的oem_auth_config.h和oem_auth_result_storage.h可能要修改为：  
"//vendor/ingenic/smartpen/adapter/kitframework:oem_auth_result_storage",   
"//vendor/ingenic/smartpen/adapter/kitframework:oem_auth_config",

### 第三步
输入：hb build -f -T //test/xts/acts/applications/kitframework_ipcamera:ActsKitFwkApiTest  
编译完成后可在out/bin目录找到ActsKitFwkApiTest.bin文件

### 第四步
放入设备，运行看日志