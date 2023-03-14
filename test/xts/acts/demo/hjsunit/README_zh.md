# 应用示例

-   [概要简介](#section_HJSUnit_001)
-   [使用说明](#section_HJSUnit_002)
-   [约束与限制](#section_HJSUnit_003)

## 概要简介<a name="section_HJSUnit_001"></a>

为帮助测试人员快速熟悉HJSUnit所提供的测试能力和如何使用，我们提供了一系列的应用示例，即Sample。每一个应用示例都是一个独立的DevEco Studio工程项目，测试人员可以将工程中的代码导入到自定义工程中，通过浏览代码、编译工程、安装和运行应用示例来了解应用示例中涉及测试能力的使用方法。

## 使用说明<a name="section_HJSUnit_002"></a>

1.  开发工具：2.2.0.200版本及以上的Deveco Studio
2.  使用步骤：①在Deveco Studio中新建空白JS模板项目，将示例工程ohosTest目录下的示例代码导入到自定义工程的ohosTest目录下，进行编译构建及运行调试。
3.  安装运行后，即可在设备上查看应用示例运行效果，以及进行相关调试。

## 约束与限制<a name="section_HJSUnit_003"></a>

安装运行应用示例之前，请先通过config.json文件中的"deviceType"字段来确认该应用示例支持的设备类型，可尝试通过修改该字段使其可以在相应类型的设备上运行（config.json文件一般在代码的entry/src/main路径下，不同的Sample可能会有不同）。