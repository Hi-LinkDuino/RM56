

## 错误使用的deps检查



**检查原因：**

在BUILD.gn中说明模块依赖的时候，需要区分部件内依赖和部件间依赖，部件内依赖在deps里，部件间依赖在external_deps里，但是有的模块的deps存在误用，deps里有部件间依赖，使用这个工具可以检查现有模块的哪些deps存在误用。

**检查步骤：**

1. 编译代码生成中间文件，只执行gn阶段，并打开check_deps属性

   ```shell
   ./build.sh --product-name Hi3516DV300 --ccache --gn-args pycache_enable=true --gn-args check_deps=true --build-only-gn 
   ```

2. 运行检查脚本

   ```shell
   # 比如在源码根目录下执行
   python build/tools/check_deps/check_deps.py --parts-path-file out/ohos-arm-release/build_configs/parts_info/parts_path_info.json --deps-path out/ohos-arm-release/deps_files
   ```

3. 脚本参数说明

   ```
   --deps-path           # 必选  模块依赖数据文件目录，gn执行完成后输出在out/ohos-arm-release/deps_files目录下，如果不在源码目录执行，需要配置到正确的路径。
   --parts-path-file     # 必选 部件路径文件，包含每个部件的路径信息
   ```

4. 输出

   脚本运行后会在 `--deps-path` 指定的deps_files的同级目录下，创建module_deps_info文件夹输出结果数据。

   ```
   module_deps_info/wrong_used_deps.json        # 生成错误使用的deps文件
   ```

**注意：**
*工具能力在完善中，使用脚本和步骤会随时变化。*
