# 编译测试案例

-   [简介]
-   [目录]
-   [约束与限制]
-   [说明]
-   [相关问题说明]

## 简介

提供了一个基于cmake编译的框架，可编译不同平台的二进制文件

根据客户的需求，编译生成对应的可执行程序和库文件。其编译流程为：

1.  使用python脚本进行配置cmake编译的参数
2.  python运行后会生成Makefile文件
3.  通过运行cmake --build .来执行编译的任务

## 目录结构

```
.
├── README.en.md
├── README.md
├── ndk-test
│   ├── CMakeLists.txt           # ndk-test通过cmake构建的起点
│   ├── build.py                 # 编译ndk-test的脚本
│   └── src
├── sanitize
│   ├── CMakeLists.txt           # sanitize通过cmake构建的起点
│   ├── asan
│   ├── build.py                 # 编译sanitize的脚本
│   ├── cfi
│   ├── fuzz
│   ├── scudo
│   └── ubsan
├── script                       # 编译放置的脚本
│   ├── __pycache__
│   ├── build.py                 # 编译的总脚本
│   ├── config_args.py           # cmake编译需要的参数
│   ├── runtest-sanitize.sh
│   ├── runtest.sh
│   └── runtest_Windows.bat      # 将编译生成的文件推送到板子上
└── target                       # 存放目标文件
    └── README.md
```

## 限制与约束

-   需要下载NDK或者通过openharmony编译生成
-   配置环境，详情请参照[gitee中的wiki文档](https://gitee.com/openharmony/third_party_musl/wikis/Home)

## 说明

1.  总目录下全量编译的编译命令

    ```
    python build.py
    ```

2.  对应的编译目录可单独编译

    ```
    # 子目录sanitize
    python build.py
    # 子目录ndk-test
    python build.py
    ```

3.  在板子上运行测试案例
    
    ```
    # 确定板子连接好电脑，运行脚本
    runtest_Windows.bat
    ```

4.  若以上步骤执行成功，脚本会产生一个REPORT和REPORT-SANITIZE文件，来分别记录ndk-test和sanitize测试案例运行的结果

## 相关问题说明

**对于cmake参数CMAKE_TOOCHAIN_FILE**
-   指向的是工具链文件所在的位置，对于编译不同平台二进制文件是很重要的，所以要给定正确的路径
 
  
