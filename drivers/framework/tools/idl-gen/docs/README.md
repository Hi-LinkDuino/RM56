# h2idl工具使用手册

- [工具简介](#工具简介)
  - [用途](#用途)
  - [需求背景](#需求背景)
- [工具使用](#工具使用)
  - [安装python](#安装python)
  - [使用方法](#使用方法)
  - [参数说明](#参数说明)
  - [命令举例](#命令举例)

  
## 工具简介
### 用途

本工具用于将C/C++头文件(.h)转换为接口描述文件(.idl)

### 需求背景

对于存量接口或者已有C/C++接口，开发者期望能提供工具可以自动将C/C++头文件转换为idl文件，以提高HDI开发效率。

## 工具使用
### 安装python
本工具需要安装python 3.8及以上版本，请根据环境选择下载对应的安装包。Python下载地址：https://www.python.org/downloads/

### 使用方法

```bash
python idl_generator.py -f <*.h> -o <directory>
```

### 参数说明
```
  -h, --help            show this help message and exit
  -v, --version         Display version information
  -f <*.h>, --file <*.h>
                        Compile the C/C++ header file
  -o <directory>, --out <directory>
                        Place generated .idl files into the <directory>
```

### 命令举例
```bash
python idl_generator.py -f ./h/audio/audio_adapter.h -o out
```