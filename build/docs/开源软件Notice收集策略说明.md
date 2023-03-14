## OpenHarmony开源软件Notice收集策略说明



### 背景

### 收集目标

只收集打包到镜像里面的模块对应的License；不打包的都不收集，比如构建过程使用的工具（如clang/python/ninja等）。

静态库本身是不会被打包的，一般是作为动态库或者可执行程序的一部分被打包到系统中的，为了确保完备，静态库的都会收集。

最终合并的Notice.txt要体现出镜像中每个文件都是用了哪些License，模块和License要有对应关系。

最终合并的Notice.txt文件在/system/etc/ 目录下。



### 收集规则

**按照优先级收集License**

1. 模块在BUILD.gn中直接声明自己使用的License文件，优先级最高。如下图示例：

   ```
   ohos_shared_library("example") {
   	...
   	license_file = "path-to-license-file"
   	...
   }
   ```

2. 如果模块没有显式声明，那么编译脚本会在BUILD.gn所在的当前目录中查找Readme.OpenSource文件，解析该文件，找出该文件中声明的license，将其作为模块的License。

   如果Readme.OpenSource文件中配置的license文件不存在，直接报错。

3. 如果Readme.OpenSource文件不存在，编译脚本会从当前目录开始，向上层目录寻找（一直找到源码的根目录），默认查找License/Copyright/Notice三个文件，如果找到，则将其作为模块的License。

4. 如果上面三种方式都未找到license，则使用默认的license作为该模块的license；默认license是Apache2.0 License。



**需要注意及检查的问题**

1. 三方的开源软件，比如openssl，icu等，这部分软件基本上在源码目录下都是要求配置Readme.OpenSource，要检查Readme.OpenSource文件是否和BUILD.gn文件在同一个目录，以及Reame.OpenSource文件中配置的License文件是否存在以及真是有效。
2. 代码目录下，如果代码使用的不是Apache2.0 License，需要在目录下提供对应的License文件，或者直接在模块中指定license_file。
3. 如果BUILD.gn中添加的源码文件不是当前目录的，需要检查下源码文件所在仓下的license是否和BUILD.gn文件所在仓的一致，不一致的需要处理。

