@echo off

@REM 本地OpenHarmony源码目录,请根据实际环境修改！！
set LOCAL=\\wsl$\ubuntu-20.04\home\OHOS\
@REM 本地编译out目录
set OUTDIR=out\hi3516dv300

@REM 本地用例生成目录
set TESTDIR=%LOCAL%\%OUTDIR%\tests\unittest\libc-test\src
@REM 动态链接库生成目录
set DYNLIB=%LOCAL%\%OUTDIR%\musl\libc-test-lib
@REM 远程传输目标目录
set REMOTE=/data/tests/libc-test/src
@REM runtest脚本所在目录
set SHDIR=%LOCAL%\third_party\musl\scripts

@REM 非必要情况下不要修改以下代码
@REM 开始时间
set /a startS=%time:~6,2%
set /a startM=%time:~3,2%

@REM 检查设备是否连接
echo HDC device checking...
for /F "usebackq delims==" %%c in (`hdc list targets`) DO (
    echo Device list:
    echo %%c | findstr "[Empty]" && goto noDevice || echo %%c && goto hdcStart
)

@REM 在单板创建目录,原先并不存在相应目录，也无法在传输时创建，因此需要预先创建好才能传输到相应位置。
:hdcStart
echo.
echo now mkdir...
hdc shell mkdir /data/tests
hdc shell mkdir /data/tests/libc-test
hdc shell mkdir %REMOTE%
hdc shell mkdir %REMOTE%/api
hdc shell mkdir %REMOTE%/common
hdc shell mkdir %REMOTE%/functional
hdc shell mkdir %REMOTE%/math
hdc shell mkdir %REMOTE%/musl
hdc shell mkdir %REMOTE%/regression

@REM 创建临时文件夹,用于存放用例生成的临时文件
hdc shell mkdir /tmp
hdc shell mkdir /dev/shm
echo Mkdir done.
goto hdcSend

@REM 传输文件,单板上执行runtest.sh，将结果REPORT返回到.bat所在目录
:hdcSend
for /F %%i in ('dir %TESTDIR% /S /B') do (
    for %%b in ("%%i\..") do (
        echo Sending %%~nb/%%~nxi
        if "%%~nb" == "src" (
        hdc shell chmod a+x %REMOTE%/%%~nxi	
        ) else (
          hdc file send -sync %%i %REMOTE%/%%~nb/%%~nxi
          hdc shell chmod a+x %REMOTE%/%%~nb/%%~nxi
        )
    )
)
@REM 动态库传输
hdc file send %DYNLIB%\libdlopen_dso.so %REMOTE%/functional/libdlopen_dso.so
hdc file send %DYNLIB%\libdlclose_reset_dso.so %REMOTE%/functional/libdlclose_reset_dso.so
hdc file send %DYNLIB%\libtls_align_dso.so %REMOTE%/functional/libtls_align_dso.so
hdc file send %DYNLIB%\libtls_init_dso.so %REMOTE%/functional/libtls_init_dso.so
hdc file send %DYNLIB%\libtls_get_new-dtv_dso.so %REMOTE%/regression/libtls_get_new-dtv_dso.so

@REM 修改动态库权限
hdc shell chmod a+x	%REMOTE%/functional/libdlopen_dso.so
hdc shell chmod a+x %REMOTE%/functional/libdlclose_reset_dso.so
hdc shell chmod a+x	%REMOTE%/functional/libtls_align_dso.so
hdc shell chmod a+x	%REMOTE%/functional/libtls_init_dso.so
hdc shell chmod a+x %REMOTE%/regression/libtls_get_new-dtv_dso.so
echo Test cases sending finished.
echo.
goto sendSH


@REM 发送脚本并执行用例
:sendSH
echo Sending runtest.sh
hdc file send %SHDIR%\runtest.sh %REMOTE%/runtest.sh
hdc shell chmod a+x %REMOTE%/runtest.sh
echo runtest.sh has been transported.
echo.
echo hdc shell .%REMOTE%/runtest.sh
hdc shell .%REMOTE%/runtest.sh
echo.
echo ================================
echo The test cases have been executed.

@REM 删除临时文件夹
hdc shell rm /tmp -rf
hdc shell rm /dev/shm -rf
echo.
echo hdc file recv %REMOTE%/REPORT %~dp0REPORT
hdc file recv %REMOTE%/REPORT %~dp0REPORT
goto end

@REM 提示检查设备连接。
:noDevice
echo Device not found,please check your device.
goto end

@REM 完成所用时间
:end
echo.
set /a endS=%time:~6,2%
set /a endM=%time:~3,2%
set /a diffS_=%endS%-%startS%
set /a diffM_=%endM%-%startM%

@REM REPORT文件比较 
start python %LOCAL%\third_party\musl\scripts\compare.py
if exist "%LOCAL%\third_party\musl\scripts\\result.html" (
  echo Test failed,please checking result.html!
) else (
  echo Test successful!
)  
echo file compareing finished

echo All items finished.
echo Time cost:%diffM_%m%diffS_%s .
echo.
pause
exit
