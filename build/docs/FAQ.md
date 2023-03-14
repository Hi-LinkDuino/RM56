
## 常见问题汇总


### 如何将一个模块编译并打包到版本中？

  - 模块要指定part\_name，指定它归属的部件，一个模块只能属于一个部件；
  - 部件的模块，要在部件配置的module\_list中，或者可以被module\_list中的模块依赖到；
  - 部件要加到对应产品的部件列表中；


### 如何指定模块在image中的安装路径，如何实现模块打包到指定image？

每一种编译模板都有缺省的打包路径，比如：
  - ohos_executable
    打包到对应镜像的bin目录下，如：`/system/bin`目录；
  - ohos_shared_library
    打包到对应镜像的lib(64)目录下, 如：`/system/lib(64)`目录;
  - ohos_prebuilt_etc
    打包到对应镜像的etc目录下, 如：`/system/etc`目录;

如果要自定义打包路径，可以在模块定义中使用参数指定：

**module_install_dir**
  模块安装路径，从system/，vendor/后开始指定

**relative_install_dir**
  模块安装相对路径，相对于模板的缺省路径，比如ohos_executable模板就是从system/bin/开始；
  
*注意：如果有module_install_dir配置时，relative_install_dir不生效。*



**模块实现打包到指定image：**

使用`install_images = []`指定模块需要打包到哪个image中，缺省为system，可以同时打包到多个分区；

比如：
  - `install_images = [ "vendor" ]`  打包到vendor.img
  - `install_images = [ "system", "vendor" ]`  同时打包到system和vendor中


### 关于--build-target使用

build-target 是传给ninja的参数，build.ninja里面的任务都可以。
  1. gn里面的一个target都可以，包括可执行程序，动态库，group，action，部件名
  2. 如果直接指定名称报unkonw target的话，可能是重名等原因，按照gn的全名称指定：{目标所在BUILD.gn的路径}:{目标名}， 比如： `--build-target utils/native/base:utils`
  3. build-target参数一次可以指定多，比如：`--build-target A --build-target B`
  4. subsystem_name不是编译目标
  5. 部件名是一个特殊的目标，根据ohos.build编译系统生产的gn目标，如果直接指定部件名，使用`--build-target {部件名}` 报错unkonw target，可以使用一个部件的全名称指定：`--build-target out/{device_name}/build_configs/{subsystem_name}/{part_name}:{part_name}`



