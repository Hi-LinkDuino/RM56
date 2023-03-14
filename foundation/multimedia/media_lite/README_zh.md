# 媒体子系统MEDIA\_LITE组件<a name="ZH-CN_TOPIC_0000001126988465"></a>

-   [简介](#section11660541593)
-   [目录](#section1829614156212)
-   [使用说明](#section1648194512427)
-   [约束](#section161941989596)
-   [相关仓](#section105062051111614)

## 简介<a name="section11660541593"></a>

提供播放、录制、解析、解码等接口能力，并提供媒体播放录制引擎服务化能力。

**图 1**  媒体子系统架构（蓝色虚线框是media\_lite）<a name="fig1737281407"></a>  
![](figures/媒体子系统架构（蓝色虚线框是media_lite）.png "媒体子系统架构（蓝色虚线框是media_lite）")

## 目录<a name="section1829614156212"></a>

```
/foundation/multimedia/media_lite
├── frameworks # 框架代码
├── interfaces # 接口
│   └── kits   # 对外接口
└── test       # 测试代码
```

## 使用说明<a name="section1648194512427"></a>

单仓的编译构建，在根目录下进行单仓的构建和编译

```
# 开发板选择
hb set  
# 单仓构建和编译
hb build media_lite
```

## 约束<a name="section161941989596"></a>

C++11版本或以上

## 相关仓<a name="section105062051111614"></a>

[媒体子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%AA%92%E4%BD%93%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[camera\_lite](https://gitee.com/openharmony/multimedia_camera_lite/blob/master/README_zh.md)

[camera\_sample\_lite](https://gitee.com/openharmony/applications_sample_camera/blob/master/README_zh.md)

[media\_utils\_lite](https://gitee.com/openharmony/multimedia_utils_lite/blob/master/README_zh.md)

[audio\_lite](https://gitee.com/openharmony/multimedia_audio_lite/blob/master/README_zh.md)

**media\_lite**

