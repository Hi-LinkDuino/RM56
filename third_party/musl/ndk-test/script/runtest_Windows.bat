@echo off


REM 本地路径和远程路径
@REM 本地OpenHarmony源码目录,请根据实际环境修改！！，脚本参数获取
set LOCAL=Z:\home\oumeng\third\third_party_musl\ndk-test\target
@REM 远程传输目标目录
set REMOTE=/data/ndk-test
@REM runtest脚本所在目录
set SHDIR=%LOCAL%\..\script

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
hdc shell mkdir %REMOTE%

@REM 修改文件夹权限
@REM hdc shell mount -o rw,remount /

@REM 创建临时文件夹,用于存放用例生成的临时文件
hdc shell mkdir /tmp
hdc shell mkdir /dev/shm
echo Mkdir done.
goto hdcSend

@REM 传输文件,单板上执行runtest.sh，将结果REPORT返回到.bat所在目录 动态库传输
:hdcSend
for /d %%i in (%LOCAL%\*) do (
    @REM echo %%i
    @REM echo %%~ni
    hdc shell mkdir %REMOTE%/%%~ni
    for /d %%s in (%%i\*) do (
        @REM echo %%~ns
        hdc shell mkdir %REMOTE%/%%~ni/%%~ns
        for %%f in (%%s\*) do (
            hdc file send %%f %REMOTE%/%%~ni/%%~ns
        )
        hdc shell chmod +x %REMOTE%/%%~ni/%%~ns/*
    )
)

echo Test cases sending finished.
echo.
goto sendSH

@REM 传输文件,单板上执行runtest.sh，将结果REPORT返回到.bat所在目录 动态库传输
:hdcSend
@REM 发送脚本并执行用例
:sendSH
echo Sending runtest.sh
hdc file send %SHDIR%\runtest.sh %REMOTE%/runtest.sh
@REM hdc file send %SHDIR%\runtest-sanitize.sh %REMOTE%/runtest-sanitize.sh
hdc shell chmod u+x %REMOTE%/runtest.sh
@REM hdc shell chmod u+x %REMOTE%/runtest-sanitize.sh
echo runtest.sh has been transported.
echo runtest-sanitize.sh has been transported.
echo.
echo hdc shell .%REMOTE%/runtest.sh
hdc shell sh %REMOTE%/runtest.sh
echo.
echo ================================
echo The test cases have been executed.

@REM 删除临时文件夹
hdc shell rm /tmp -rf
hdc shell rm /dev/shm -rf
echo.
echo hdc file recv %REMOTE%/REPORT %~dp0REPORT
hdc file recv %REMOTE%/REPORT %~dp0REPORT
@REM echo hdc file recv %REMOTE%/REPORT-SANITIZE %~dp0REPORT-SANITIZE
@REM hdc file recv %REMOTE%/REPORT-SANITIZE %~dp0REPORT-SANITIZE
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
@REM start python %LOCAL%\third_party\musl\scripts\compare.py
@REM if exist "%LOCAL%\third_party\musl\scripts\\result.html" (
@REM   echo Test failed,please checking result.html!
@REM ) else (
@REM   echo Test successful!
@REM )  
@REM echo file compareing finished

@REM echo All items finished.
@REM echo Time cost:%diffM_%m%diffS_%s .
@REM echo.
pause
exit
