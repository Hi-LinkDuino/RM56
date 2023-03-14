## libc-test脚本使用说明

### 一、生成用例

编译命令，单独编译出libc-test测试用例

```shell
./build.sh --product-name Hi3516DV300 --build-target third_party/musl:libctest
```

文件生成的目录位于  `out/hi3516dv300/tests/unittest/libc-test`

注：

旧版ohos中，文件生成的目录位于  `out/ohos-arm-release/tests/unittest/libc-test`，请根据实际修改脚本中相关路径！！

请务必根据实际修改脚本中相关路径！

### 二、执行用例

1. 将HDC(Windows版)工具加入到windows环境变量,并将工具改名为hdc.exe。连接上开发板，保证HDC工具可以连接。

2. 进入musl/scripts，并根据实际环境，修改runtest.bat中测试用例生成目录完整路径（必要）

   ```bat
   @REM 本地OpenHarmony源码目录,请根据实际环境修改！！
   set LOCAL=\\wsl$\ubuntu-20.04\home\OHOS\
   @REM 本地编译out目录
   set OUTDIR=out\hi3516dv300
   ```
   
3. 将bat脚本复制到windows环境下使用！！windows端双击运行runtest.bat，执行完毕会将结果REPORT返回到当前bat所在目录。

   此bat将从WSL映射目录传输文件到单板目录下，由于WSL无法直接访问windows端口，所以在WSL中直接使用可能无法传输用例。

4. 若要单独执行测试用例，需要先将common目录下的`runtest`与用例传输到单板，再通过命令执行，结果将打印出来

   ```shell
   ./runtest -w '' {filename}
   ```
5. runtest.bat执行结束前会将输出的REPORT与在libc-test/report_base下的REPORT相比较，如果比较出不同将会在scripts下
   生成一个result.html文件，否则不生成。
   

### 三、增加用例

若要在functional、math、regression或增加用例，需要在相应目录的test_src_{dir}.gni中添加文件名。

例如：

1、需要在functional目录中新增一个用例：由`new_test.c`生成可执行文件`new_test`

2、首先把c文件放入functional目录中，打开`test_src_functional.gni`，在`functional_list`中最后一行增加`"new_test",`

3、执行步骤一（生成用例）中的编译命令，即可生成对应可执行文件。

