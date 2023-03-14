# 应用权限管理<a name="ZH-CN_TOPIC_0000001101239136"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [约束](#section119744591305)
-   [使用](#section137768191623)
    -   [接口说明](#section1551164914237)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1371113476307) 
## 简介<a name="section11660541593"></a>

OpenHarmony中应用和系统服务均运行在独立的沙箱中，进程空间和程序数据都是相互隔离的，以保护应用数据的安全性；但是运行在独立沙箱中的服务或应用同时需要对外提供一些API以实现所需功能，其他独立沙箱中的应用在跨进程访问这些API时，需要系统提供一种权限管理机制对这些API的访问者进行授权。

-   应用权限管理提供了权限定义机制，允许系统服务和应用为自己的敏感API定义新的权限，其他应用必须申请此权限才能访问此敏感API；

-   应用权限管理提供了权限申请机制，允许应用申请权限，这些权限由系统或者其他应用定义，权限申请通过后就能访问这个权限相关的系统或其他应用提供的敏感API；
-   应用权限管理也为用户提供了一些必须的功能，方便用户查看和管理权限授予情况。

**图 1**  应用权限管理架构<a name="fig4460722185514"></a>


![](figures/zh-cn_image_0000001113598272.png)

应用权限管理为用户程序框架子系统提供权限管理功能，并且为上层应用提供权限申请和授权状态查询接口。本次开源的应用权限管理功能适用于轻量系统、小型系统和标准系统。

-   轻量系统（mini system）面向MCU类处理器例如Arm Cortex-M、RISC-V 32位的设备，硬件资源极其有限，参考内存≥128KiB，可以提供多种轻量级网络协议，轻量级的图形框架，以及丰富的IOT总线读写部件等。可支撑的产品如智能家居领域的连接类模组、传感器设备、穿戴类设备等。
-   小型系统（small system）面向应用处理器例如Arm Cortex-A的设备，参考内存≥1MiB，可以提供更高的安全能力、标准的图形框架、视频编解码的多媒体能力。可支撑的产品如智能家居领域的IP Camera、电子猫眼、路由器以及智慧出行域的行车记录仪等。
-   标准系统（standard system）面向应用处理器例如Arm Cortex-A的设备，参考内存≥128MiB，可以提供增强的交互能力、3D GPU以及硬件合成能力、更多控件以及动效更丰富的图形能力、完整的应用框架。可支撑的产品如高端的冰箱显示屏。

## 目录<a name="section161941989596"></a>

```
/base/security/permission
├── frameworks                         # 基础设施层
│   └── permission_standard            # 标准系统权限管理基础设施层
├── interfaces                         # 接口层
│   ├── innerkits                      # 内部接口层
│   │   ├── permission_lite            # 轻量系统、小型系统权限管理内部接口层
│   │   └── permission_standard        # 标准系统权限管理内部接口层
│   └── kits                           # 外部接口层
│       ├── permission_lite            # 轻量系统、小型系统权限管理外部接口层
│       └── permission_standard        # 标准系统权限管理外部接口层
└── services                           # 服务层
    ├── permission_lite                # 轻量系统、小型系统权限管理服务层
    └── permission_standard            # 标准系统权限管理服务层
```

## 约束<a name="section119744591305"></a>

-   标准系统应用权限管理本期仅提供本地权限管理的C++接口，不涉及分布式权限管理。

## 使用<a name="section137768191623"></a>

### 接口说明<a name="section1551164914237"></a>

**标准系统应用权限管理**：此模块主要为标准系统用户程序框架子系统提供权限管理基础校验能力，不对三方app开放，并提供如下API：

<a name="table17351104911243"></a>
<table><thead align="left"><tr id="row43512497244"><th class="cellrowborder" valign="top" width="73.41%" id="mcps1.1.3.1.1"><p id="p8351104918247"><a name="p8351104918247"></a><a name="p8351104918247"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="26.590000000000003%" id="mcps1.1.3.1.2"><p id="p7351174913247"><a name="p7351174913247"></a><a name="p7351174913247"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row143511494244"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p2504174918322"><a name="p2504174918322"></a><a name="p2504174918322"></a>int VerifyPermission(const string&amp; bundleName, const string&amp; permissionName, int userId)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p14504549163217"><a name="p14504549163217"></a><a name="p14504549163217"></a>校验应用是否已授予对应的权限</p>
</td>
</tr>
<tr id="row217303717326"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p8504849123210"><a name="p8504849123210"></a><a name="p8504849123210"></a>bool CanRequestPermission(const string&amp; bundleName, const string&amp; permissionName, int userId)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p850417499329"><a name="p850417499329"></a><a name="p850417499329"></a>判断应用是否需要弹框申请权限</p>
</td>
</tr>
<tr id="row677573713220"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p16504124933210"><a name="p16504124933210"></a><a name="p16504124933210"></a>int GrantUserGrantedPermission(const string&amp; bundleName, const string&amp; permissionName, int userId)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p115042494327"><a name="p115042494327"></a><a name="p115042494327"></a>授予应用对应的user_grant权限</p>
</td>
</tr>
<tr id="row722533813329"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p9504114963210"><a name="p9504114963210"></a><a name="p9504114963210"></a>int GrantSystemGrantedPermission(const string&amp; bundleName, const string&amp; permissionName)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p850412493329"><a name="p850412493329"></a><a name="p850412493329"></a>授予应用对应的system_grant权限</p>
</td>
</tr>
<tr id="row1354353873216"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p450414919328"><a name="p450414919328"></a><a name="p450414919328"></a>int RevokeUserGrantedPermission(const string&amp; bundleName, const string&amp; permissionName, int userId)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p1050411498327"><a name="p1050411498327"></a><a name="p1050411498327"></a>撤销应用对应的user_grant权限</p>
</td>
</tr>
<tr id="row1073519380323"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p1850484933212"><a name="p1850484933212"></a><a name="p1850484933212"></a>int RevokeSystemGrantedPermission(const string&amp; bundleName, const string&amp; permissionName)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p35048492326"><a name="p35048492326"></a><a name="p35048492326"></a>撤销应用对应的system_grant权限</p>
</td>
</tr>
<tr id="row1692163820325"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p9504134953215"><a name="p9504134953215"></a><a name="p9504134953215"></a>int AddUserGrantedReqPermissions(const string&amp; bundleName, const std::vector&lt;string&gt;&amp; permList, int userId)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p250424993212"><a name="p250424993212"></a><a name="p250424993212"></a>添加应用申请的user_grant权限</p>
</td>
</tr>
<tr id="row1890399325"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p1950413498329"><a name="p1950413498329"></a><a name="p1950413498329"></a>int AddSystemGrantedReqPermissions(const string&amp; bundleName, const std::vector&lt;string&gt;&amp; permList)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p1150444943210"><a name="p1150444943210"></a><a name="p1150444943210"></a>添加应用申请的system_grant权限</p>
</td>
</tr>
<tr id="row13257153973215"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p2505184917329"><a name="p2505184917329"></a><a name="p2505184917329"></a>int RemoveUserGrantedReqPermissions(const string&amp; bundleName, int userId)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p950514973212"><a name="p950514973212"></a><a name="p950514973212"></a>移除应用申请的所有user_grant权限</p>
</td>
</tr>
<tr id="row144437398322"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p205051049103214"><a name="p205051049103214"></a><a name="p205051049103214"></a>int RemoveSystemGrantedReqPermissions(const string&amp; bundleName)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p125051349123213"><a name="p125051349123213"></a><a name="p125051349123213"></a>移除应用申请的所有system_grant权限</p>
</td>
</tr>
<tr id="row13617183915329"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p16505049173217"><a name="p16505049173217"></a><a name="p16505049173217"></a>int AddDefPermissions(const std::vector&lt;PermissionDef&gt;&amp; permList)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p13505849103215"><a name="p13505849103215"></a><a name="p13505849103215"></a>添加应用定义的权限</p>
</td>
</tr>
<tr id="row117857394324"><td class="cellrowborder" valign="top" width="73.41%" headers="mcps1.1.3.1.1 "><p id="p105059492325"><a name="p105059492325"></a><a name="p105059492325"></a>int GetDefPermission(const string&amp; permissionName, PermissionDef&amp; permissionDefResult)</p>
</td>
<td class="cellrowborder" valign="top" width="26.590000000000003%" headers="mcps1.1.3.1.2 "><p id="p11505349103220"><a name="p11505349103220"></a><a name="p11505349103220"></a>获取指定权限名的权限定义信息</p>
</td>
</tr>
</tbody>
</table>
**轻量系统、小型系统应用权限管理**：当前仅供系统应用和系统服务调用，具体API接口如下：

<a name="table9789027162518"></a>
<table><thead align="left"><tr id="row9789427112518"><th class="cellrowborder" valign="top" width="55.66%" id="mcps1.1.3.1.1"><p id="p9790102717251"><a name="p9790102717251"></a><a name="p9790102717251"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="44.34%" id="mcps1.1.3.1.2"><p id="p779032715251"><a name="p779032715251"></a><a name="p779032715251"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row187901627112516"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p138014275353"><a name="p138014275353"></a><a name="p138014275353"></a>int CheckPermission(int uid, const char *permissionName)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p57901727202510"><a name="p57901727202510"></a><a name="p57901727202510"></a>检查指定UID的应用进程是否具有访问系统服务API的权限</p>
</td>
</tr>
<tr id="row19341734164410"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p123453412448"><a name="p123453412448"></a><a name="p123453412448"></a>int CheckSelfPermission(const char *permissionName)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p937645212447"><a name="p937645212447"></a><a name="p937645212447"></a>检查调用者是否具有访问系统服务API的权限</p>
</td>
</tr>
<tr id="row879032715258"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p13071135133514"><a name="p13071135133514"></a><a name="p13071135133514"></a>int QueryPermission(const char *identifier, PermissionSaved **permissions, int *permNum)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p1379072718259"><a name="p1379072718259"></a><a name="p1379072718259"></a>查询应用申请的所有权限，并检查权限是否被授予</p>
</td>
</tr>
<tr id="row877239193516"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p878133903516"><a name="p878133903516"></a><a name="p878133903516"></a>int GrantPermission(const char *identifier, const char *permName)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p678143943515"><a name="p678143943515"></a><a name="p678143943515"></a>将指定权限授予应用程序</p>
</td>
</tr>
<tr id="row3616164223510"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p1617142163517"><a name="p1617142163517"></a><a name="p1617142163517"></a>int RevokePermission(const char *identifier, const char *permName)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p06171242143517"><a name="p06171242143517"></a><a name="p06171242143517"></a>收回应用程序的指定权限</p>
</td>
</tr>
<tr id="row13790122742516"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p23273123365"><a name="p23273123365"></a><a name="p23273123365"></a>int GrantRuntimePermission(int uid, const char *permissionName)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p177908273259"><a name="p177908273259"></a><a name="p177908273259"></a>应用运行时动态授予指定权限</p>
</td>
</tr>
<tr id="row18566191217452"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p169891916194512"><a name="p169891916194512"></a><a name="p169891916194512"></a>int RevokeRuntimePermission(int uid, const char *permissionName)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p937132011440"><a name="p937132011440"></a><a name="p937132011440"></a>应用运行时动态撤销指定权限</p>
</td>
</tr>
<tr id="row18566191217452"><td class="cellrowborder" valign="top" width="55.66%" headers="mcps1.1.3.1.1 "><p id="p169891916194512"><a name="p169891916194512"></a><a name="p169891916194512"></a>int UpdatePermissionFlags(const char *identifier, const char *permissionName, const int flags)</p>
</td>
<td class="cellrowborder" valign="top" width="44.34%" headers="mcps1.1.3.1.2 "><p id="p937132011440"><a name="p937132011440"></a><a name="p937132011440"></a>更新应用程序指定的应用权限的标记</p>
</td>
</tr>
</tbody>
</table>


**轻量系统、小型系统IPC通信鉴权**：

<a name="table10494122145517"></a>

<table><thead align="left"><tr id="row1494152195511"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p14941221135515"><a name="p14941221135515"></a><a name="p14941221135515"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p8494172116555"><a name="p8494172116555"></a><a name="p8494172116555"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1849482118555"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1414381815720"><a name="p1414381815720"></a><a name="p1414381815720"></a>int GetCommunicationStrategy(RegParams params, PolicyTrans **policies, unsigned int *policyNum)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p749582195510"><a name="p749582195510"></a><a name="p749582195510"></a>服务注册过程中查询调用接口对应的访问策略，仅供Samgr调用</p>
</td>
</tr>
<tr id="row8495521115517"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p966319247576"><a name="p966319247576"></a><a name="p966319247576"></a>int IsCommunicationAllowed(AuthParams params)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p134951921115511"><a name="p134951921115511"></a><a name="p134951921115511"></a>检查访问主体进程是否有权限调用受访客体进程的接口，仅供Samgr调用</p>
</td>
</tr>
</tbody>
</table>

### 使用说明<a name="section129654513264"></a>

**标准系统应用权限管理**：

使用说明：所有接口均为内部接口，仅提供底层能力，不对开发者开放。鉴权过程中直接调用VerifyPermission接口即可。

1.  明确要校验应用的UID及需要校验的权限名称permissionName
2.  根据UID获取应用的包名 bundleName
3.  根据UID获取应用的用户ID userId
4.  将需要校验的权限名permissionName, 包名bundleName和userId传入接口VerifyPermission\(string permissionName, string bundleName, int userId\)
5.  得到校验结果

**轻量系统、小型系统应用权限管理**：

使用说明：以包管理器的应用权限开发为例进行讲解。开发过程中，首先需要明确涉及的敏感权限，并在config.json中声明该权限，在安装应用程序时，包管理器会调用应用权限管理组件的接口检查该权限是否被授予，若授予，安装流程正常进行，否则安装失败。

1.  在开发过程中，包管理器明确需要安装应用的权限（ohos.permission.INSTALL\_BUNDLE），并在config.json中声明该权限；

    ```
    {
      ...
      "module": {
          "package": "ohos.demo.kitframework",
          "deviceType": [
              "phone", "tv","tablet", "car","smartWatch","sportsWatch","smartCamera", "smartVision"
          ],
          "reqPermissions": [{
            // 声明需要的权限：安装应用程序的权限名
            "name": "ohos.permission.INSTALL_BUNDLE",
            "reason": "install bundle",
            "usedScene": {
              "ability": [
                "KitFramework"
                ],
              "when": "always"
            }
          },
          {
            "name": "ohos.permission.LISTEN_BUNDLE_CHANGE",
            "reason": "install bundle",
            "usedScene": {
              "ability": [
                "KitFramework"
                ],
              "when": "always"
            }
          },
          {
            "name": "ohos.permission.GET_BUNDLE_INFO",
            "reason": "install bundle",
            "usedScene": {
              "ability": [
                "KitFramework"
                ],
              "when": "always"
            }
          }
          ],
        ...
    }
    ```

2.  当包管理器开发应用安装功能接口时，会调用权限管理相关接口检查自身是否具有安装应用程序的权限，例如：以安装应用的权限名"ohos.permission.INSTALL\_BUNDLE"作为入参，调用CheckPermission接口检查包管理器是否具有安装应用的权限，如果有权限，安装流程继续执行，否则返回安装失败；

    ```
    constexpr static char PERMISSION_INSTALL_BUNDLE[] = "ohos.permission.INSTALL_BUNDLE";
    
    bool Install(const char *hapPath, const InstallParam *installParam, InstallerCallback installerCallback)
    {
        if ((hapPath == nullptr) || (installerCallback == nullptr) || (installParam == nullptr)) {
            HILOG_ERROR(HILOG_MODULE_APP, "BundleManager install failed due to nullptr parameters");
            return false;
        }
        // 检查ohos.permission.INSTALL_BUNDLE权限是否被授予
        if (CheckPermission(0, static_cast<const char *>(PERMISSION_INSTALL_BUNDLE)) != GRANTED) {
            HILOG_ERROR(HILOG_MODULE_APP, "BundleManager install failed due to permission denied");
            return false;  // 返回安装失败
        }
        // 安装流程
        ...
    }
    ```


**轻量系统、小型系统IPC通信鉴权**：

使用说明：以BMS服务通过IPC通信方式对外开放接口为例，讲解如何通过IPC通信鉴权组件配置对应接口的访问策略。这里BMS在Samgr中注册的service为bundlems，为开放的接口注册的Feature为BmsFeature。

1.  在源码路径下的头文件base/security/permission/services/permission\_lite/ipc\_auth/include/policy\_preset.h中配置相应的访问策略，访问策略主要有三种类型：

    （1）type为RANGE类型：允许某个特定范围UID的进程访问，需要指定uidMin和uidMax；

    （2）type为FIXED类型：允许指定的几个UID的进程访问，需要指定fixedUid，最多配置8个；

    （3）type为BUNDLENAME类型：只允许特定的应用访问，需要指定bundleName（包名）；

    ```
    FeaturePolicy bmsFeature[] = {
        {
            "BmsFeature",
            {
                {
                    .type=FIXED,    // 允许指定UID的进程访问的方式
                    .fixedUid={2, 3, 8}
                },
                {
                    .type=RANGE,    // 允许特定范围内的UID的进程访问的方式
                    .uidMin=100,
                    .uidMax=__INT_MAX__,
                },
            }
        },
        {
            "BmsInnerFeature",
            {
                {
                    .type=FIXED,     // 允许指定UID的进程访问的方式
                    .fixedUid={2, 3, 8}
                },
                {
                    .type=RANGE,
                    .uidMin=100,
                    .uidMax=999,
                },
            }
        },
    };
    ```

2.  将步骤1中定义的Feature的策略加配到全局策略中，需要配置feature数量；

    ```
    static PolicySetting g_presetPolicies[] = {
        {"permissionms", pmsFeature, 1},
        {"abilityms", amsFeature, 2},
        {"bundlems", bmsFeature, 2},  // 步骤1定义的BMS的feature，数量为2
        {"dtbschedsrv", dmsFeature, 1},
        {"samgr", samgrFeature, 1},
        {"appspawn", appspawnFeature, 1},
        {"WMS", wmsFeature, 1},
        {"bundle_daemon", bdsFeature, 1},
    };
    ```

3.  将步骤1中定义的BmsFeature注册到Samgr；

    ```
    const char BMS_SERVICE[] = "bundlems";
    const char BMS_FEATURE[] = "BmsFeature";
    static void Init()
    {
        SamgrLite *sm = SAMGR_GetInstance();
        if (sm == nullptr) {
            return;
        }
        // 注册服务到Samgr
        sm->RegisterFeature(BMS_SERVICE, reinterpret_cast<Feature *>(BundleMsFeature::GetInstance()));
        sm->RegisterFeatureApi(BMS_SERVICE, BMS_FEATURE,
            GetBmsFeatureApi(reinterpret_cast<Feature *>(BundleMsFeature::GetInstance())));
        HILOG_DEBUG(HILOG_MODULE_APP, "BundleMS feature start success");
    }
    APP_FEATURE_INIT(Init);
    ```


完成以上开发步骤后，开发者在Samgr注册服务时，Samgr会调用IPC通信鉴权组件的GetCommunicationStrategy接口获取服务的访问策略；当其他服务或应用通过IPC方式访问这些服务时，Samgr会调用IPC通信鉴权组件的IsCommunicationAllowed接口检查调用者服务的权限，如果满足访问策略，则可以访问开发者接口，否则拒绝访问。

## 相关仓<a name="section1371113476307"></a>
安全子系统
security\_permission

