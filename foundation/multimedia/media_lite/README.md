# MEDIA\_LITE<a name="EN-US_TOPIC_0000001126988465"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1829614156212)
-   [Usage](#section1648194512427)
-   [Constraints](#section161941989596)
-   [Repositories Involved](#section105062051111614)

## Introduction<a name="section11660541593"></a>

This repository provides APIs for media-related capabilities such as recording, playback, parsing, and decoding, and the engine capability for media recording and playback.

**Figure  1**  Position of this repository \(in the dotted blue box\) in the multimedia subsystem<a name="fig1737281407"></a>  
![](figures/position-of-this-repository-(in-the-dotted-blue-box)-in-the-multimedia-subsystem.png "position-of-this-repository-(in-the-dotted-blue-box)-in-the-multimedia-subsystem")

## Directory Structure<a name="section1829614156212"></a>

```
/foundation/multimedia/media_lite
├── frameworks # Framework code
├── interfaces # APIs
│   └── kits   # External APIs
└── test       # Test code
```

## Usage<a name="section1648194512427"></a>

Build a single repository in the root directory.

```
# Select the development board.
hb set  
# Build this repository.
hb build media_lite
```

## Constraints<a name="section161941989596"></a>

C++ 11 or later

## Repositories Involved<a name="section105062051111614"></a>

[Multimedia subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/multimedia.md)

[camera\_lite](https://gitee.com/openharmony/multimedia_camera_lite/blob/master/README.md)

[camera\_sample\_lite](https://gitee.com/openharmony/applications_sample_camera/blob/master/README.md)

[media\_utils\_lite](https://gitee.com/openharmony/multimedia_utils_lite/blob/master/README.md)

[audio\_lite](https://gitee.com/openharmony/multimedia_audio_lite/blob/master/README.md)

**media\_lite**

