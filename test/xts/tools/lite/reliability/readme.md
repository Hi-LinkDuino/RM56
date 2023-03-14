# 稳定性测试工具<a name="ZH-CN_TOPIC_0000001115694144"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [约束](#section119744591305)
-   [说明](#section06487425716)
    -   [反复开关机压力测试执行步骤使用说明](#section129654513264)
    -   [Native的测试用例进行反复压力测试使用说明](#section129654513265)
    -   [JS的测试用例进行反复压力测试使用说明](#section129654513266)

## 简介<a name="section11660541593"></a>

稳定性测试工具提供了一系列针对openharmony系统进行压力测试的方法，以便能够发现系统级、模块级的一些低概率的稳定性问题。

支持特性：

-   支持对系统整机进行反复开关机压力测试。
-   支持对Native的测试用例进行反复压力测试。
-   支持对js的测试用例进行反复压力测试。

## 目录<a name="section161941989596"></a>

```
xts_tools/reliability
├── utils.py           # 测试需要的基础方法
├── RebootPressTest.py      # 执行反复开关机压力测试的工具
├── NativePressTest.py # 执行Native的测试用例进行反复压力测试的工具
├── JSPressTest.js     # 执行JS接口的的测试用例进行反复压力测试的工具
```
## 约束<a name="section119744591305"></a>

测试工具环境依赖

1.  python版本\>=3.7.5
2.  运行操作系统：Windows版本\>=Win10，Linux为Ubuntu18.04

## 说明<a name="section06487425716"></a>

## 反复开关机压力测试执行步骤<a name="section129654513264"></a>
1.  修改测试工具代码
    RebootPressTest.py文件中关键部分介绍如下
```
    DeviceKey = "1501003847544634520655ef935dc727"  # 被测设备的设备识Key，通过hdc_std list targets命令获取
    hdcPath = "D:\\tools\\hdc\\hdc_std.exe"         # hdc_std工具目录
    rebootLoop = 300          # 总测试次数，一般建议个人级测试执行300次
    rebootWaitTime = 60       # 设备重启至恢复正常等待时间
    rebootMaxWaitTime = 300   # 设备重启至恢复正常最大等待时间，一般超过这个时间则认为设备已经出现异常，需要定位问题
```
2.  执行反复开关机压力测试
    执行python RebootPressTest.py开始测试
3.  观察测试结果
    执行过程中会打印过程中的一些日志信息，正常如下所示
```
  ------------------------TEST START---------------------------
  cmdRet is 1501003847544634520655ef935dc727
  cmdRet is start ability successfully.
  Current Time is:2021-07-03 17:20:45.962538
  Will reboot round 1, total time is 300
```
  如果执行出现了异常，则会抛出异常信息
```
  The device xxx is not ready after reboot for 300 seconds.  #重启后系统长时间不开机
  或 The device xxx can not start APP after reboot.  #重启后系统关键能力不可用
```  
## Native的测试用例进行反复压力测试执行步骤<a name="section129654513265"></a>
1.  按照https://gitee.com/openharmony/xts_acts#%E7%94%A8%E4%BE%8B%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC 链接的描述设计可靠性测试用例并完成编译
2.  修改测试工具代码
    NativePressTest.py文件中关键部分介绍如下
```
    hdcPath = "D:\\tools\\hdc\\hdc_std.exe" #hdc工具存放路径
    testFilePath = "d:\\test\\"             #测试用例存放路径
    testFile = "hilogtest"                  #测试用例文件名
    testTime = 6 * 3600                     #测试时间长度，默认测试6小时
    processNum = 5                          #测试线程数量，默认5个线程
```
2.  执行Native的测试用例进行反复压力测试
    执行python NativePressTest.py开始测试
3.  观察测试结果
    执行过程中会打印过程中的一些日志信息，正常如下所示
```
  ------------------------TEST START---------------------------
C:\Users\Administrator\AppData\Local\Programs\Python\Python39\python.exe D:/test/pyhikey/NativePressTest.py
abs path is D:\test\pyhikey
Start to PressTest Process with cmd start D:\tools\hdc\hdc_std.exe shell "cd /data/local/tmp/;/data/local/tmp/hilogtest --gtest_repeat=-1 > /dev/null "
```
  如果执行出现了异常，则会抛出异常信息，一般有两类异常：测试用例本身异常退出、系统中其他进程异常退出
```
Exception found after Test,please check faultlog path
Traceback (most recent call last):
  File "D:\test\pyhikey\NativePressTest.py", line 126, in <module>
    raise Exception("Exception found after Test,please check faultlog path")
Exception: Exception found after Test,please check faultlog path

Process finished with exit code 1
```
问题日志：Native稳定性测试完成之后，异常日志会从设备中导出并保存在NativePressTest.py文件所在目录下的faultlog目录下，请根据日志中的调用栈信息进行问题定位。

## JS的测试用例进行反复压力测试执行步骤<a name="section129654513266"></a>
1.  按照https://gitee.com/openharmony/xts_acts#js%E8%AF%AD%E8%A8%80%E7%94%A8%E4%BE%8B%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC%E9%80%82%E7%94%A8%E4%BA%8E%E6%A0%87%E5%87%86%E7%B3%BB%E7%BB%9F 链接的描述设计JS可靠性测试用例及其测试环境
2.  将JSPressTest.js测试工具放到测试用例工程的pages/index目录下，和index.js同目录，目录及文件效果如下图所示
```
├── BUILD.gn   
│ └──entry
│ │ └──src
│ │ │ └──main
│ │ │ │ └──js
│ │ │ │ │ └──default               
│ │ │ │ │ │ └──pages
│ │ │ │ │ │ │ └──index             
│ │ │ │ │ │ │ │ └──index.js        # 入口文件
│ │ │ │ │ │ │ │ └──JSPressTest.js  # JS测试用可靠性测试工具
│ │ │ │ │ └──test                  # 测试代码存放目录  
```

3.  在index.js文件中添加工具依赖，将可靠性测试工具引入，如下所示
// 引入可靠性测试工具
import {Core, ExpectExtend} from 'deccjsunit/index'
import {PressureTest} from './JSPressTest'

3.  在index.js文件的onShow方法下，添加如下的代码，并将功能测试入口注释掉，如下所示
```
    onShow() {
        console.info('onShow finish')
        const core = Core.getInstance()
        const expectExtend = new ExpectExtend({
            'id': 'extend'
        })
        core.addService('expect', expectExtend)
        core.init()
        const configService = core.getDefaultService('config')
        configService.setConfig(this)
        require('../../../test/List.test')
        //core.execute()  #将功能测试入口注释掉
        var press = new PressureTest(core);
        press.setPressTimes(10000);
        press.run();
```

4.  按照标准系统的测试用例执行方法执行测试即可

FAQ:
1.  按照测试时长维度设置压测时间
    调用接口press.setPressTimeout(timeout: int)设置压测时间，单位s
```
        var press = new PressureTest(core);
        press.setPressTimeout(10000);
        press.run();
```

2.  按照测试次数维度设置压测次数
    调用接口press.setPressTimes(times: int)设置压测时间，单位s
```
        var press = new PressureTest(core);
        press.setPressTimes(10000);
        press.run();
```

3.  测试结果检查
    测试完成之后，检查/data/log/faultlog/temp/目录下是否有cppcrash日志，如果有异常即测试过程中出现了异常