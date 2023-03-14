@echo off
setlocal enabledelayedexpansion enableextensions


set cwd=%~dp0

echo > %cwd%.com
call %cwd%select_com.bat %cwd%.com

for /f "tokens=1,2 delims==" %%i in ('type %cwd%.com') do (
  set %%i=%%j
  if %%i == "COM" ( goto :next)
)
:next

TITLE Burning COM=%COM% @(%cwd%)
if "%COM%" == "" ( pause && goto :eof )
echo COM=%COM%

set dldtool=%cwd%dld\dldtool.exe
echo Using %dldtool%
set buildinfo=%cwd%.buildinfo
if not exist %buildinfo% (
  echo %buildinfo% not exists, please rebuild
  pause
  exit
)

rem set progRate=1500000
set progRate=3000000
set COM_OPT=--pgm-rate %progRate% 

@echo Read buildinfo...
@echo =====================================
for /f "tokens=1,2 delims==" %%i in (%buildinfo%) do (
  echo     %%i=%%j
  set %%i=%%j
)
@echo =====================================

TITLE Burning CHIP=%CHIP% COM=%COM% @(%cwd%)
if "%BEST2003_TYPE%" NEQ "" (
  TITLE Burning CHIP=%CHIP% %BEST2003_TYPE% @COM=%COM% @(%cwd%)
)


cd %cwd%

@REM =================================================
@REM prepare dld args
@REM =================================================
set binPath=.
set littlefs=%binPath%\littlefs.bin
if "%BEST2003_TYPE%" NEQ "" (
  set littlefs=%binPath%\littlefs_%BEST2003_TYPE%.bin
)
set ota_boot1=%binPath%\ota_boot1.bin
set ota_boot2a=%binPath%\ota_boot2a.bin
set tz_main=%binPath%\trustzone_main.bin
set tz_mini=%binPath%\trustzone_mini.bin
set rtos_main=%binPath%\rtos_main.bin
set rtos_mini=%binPath%\rtos_mini.bin
set factory=%binPath%\factory.bin
set pub_otp=%binPath%\pub_otp.bin
set mmcfs=%binPath%\mmcfs.bin
set sensor=%binPath%\sensor_hub_watch.bin


set /a fwAddr=FLASH_NC_BASE + OTA_CODE_OFFSET
set /a fwMiniAddr=FLASH_NC_BASE + FW_MINI_CODE_OFFSET
set /a boot2Addr=%FLASH_NC_BASE% + %BOOT1_SIZE%
set /a tzMainAddr=%FLASH_NC_BASE% + %BOOT1_SIZE% + %BOOT2_SIZE% + %BOOT2_SIZE%
set /a tzMiniAddr=%FLASH_NC_BASE% + %BOOT1_SIZE% + %BOOT2_SIZE% + %BOOT2_SIZE% + %TZ_SIZE%
set /a fsaddr=%FLASH_NC_BASE% + %LFS_FLASH_BASE_ADDR%

if "%CHIP%" == "best2003" (
  set /a factoryAddr=%FLASH_NC_BASE% + %FACTORY_SECTION_OFFSET% + %FACTORY_SECTOR_OFFSET%
  set /a factory2Addr=%FLASH_NC_BASE% + %FACTORY_SECTION_OFFSET% + %FACTORY_SECTION_SIZE%/2 + %FACTORY_SECTOR_OFFSET%
) else (
  set /a factoryAddr=%FLASH_NC_BASE% + %RESERVED_SECTION_OFFSET% + %FACTORY_SECTOR_OFFSET%
  set /a factory2Addr=%FLASH_NC_BASE% + %FACTORY_SECTION_OFFSET% + %FACTORY_SECTOR_OFFSET%
)



::echo boot2Addr=%boot2Addr%
::echo fwAddr=%fwAddr%
::echo fwMiniAddr=%fwMiniAddr%
::echo fsaddr=%fsaddr%
::echo factoryAddr=%factoryAddr%


set boot2Args=--addr %boot2Addr% %ota_boot2a% -m %boot2Addr%
set boot2RemapArgs=--addr %boot2Addr% --remap-both %BOOT2_SIZE% %ota_boot2a% -m %boot2Addr% 
set tzArgs=--addr %tzMainAddr% %tz_main% -m %tzMainAddr% --addr %tzMiniAddr% %tz_mini% -m %tzMiniAddr%
set fwArgs=--addr %fwAddr% %rtos_main% -m %fwAddr%
set fwMiniArgs=--addr %fwMiniAddr% %rtos_mini% -m %fwMiniAddr%
set fwRemapArgs=--addr %fwAddr% --remap-both %OTA_CODE_SIZE% %rtos_main% -m %fwAddr%
set factArgs=--addr %factoryAddr% %factory%
set fact2Args=--addr %factory2Addr% %factory%
set fsArgs=--addr %fsaddr% %littlefs%
set mmcArgs=--addr 0x3 %mmcfs%



@REM =================================================
@REM call to feed menu list for different chip types
@REM =================================================
set lastMenuIndex=0
call :%CHIP%_setMenu

@REM =================================================
@REM Show Menu
:showMenu

cls
color 0A
echo ===================================================
echo Please input the following option for programing
echo ===================================================

echo   0. Quit

for /l %%i in (0,1,%lastMenuIndex%) do (
  set /a no=%%i+1
  echo   !no!. !menuList[%%i]!
)
echo ===================================================

:: prompt user input characers to choice variable
:choicePrompt
set choice=
set /p choice= Please input option(default 1):

IF NOT "%choice%"=="" (
  SET choice=%choice:~0,1%
  set /a c = %choice%
) else (
  set c=1
)

::echo You choose !c!
if !c! EQU 0 goto :eof
set /a c=%c%-1
if !c! GTR %lastMenuIndex% (
  echo Out of range
  goto :choicePrompt
)
if !c! LSS 0 (
  echo Out of range
  goto :choicePrompt
)

::echo goto %CHIP%_!c!
goto :%CHIP%_!c!


:best1502x

:best1502x_setMenu
set menuList[0]=rtos
set menuList[1]=littlefs(norflash filesystem)
set menuList[2]=sensor hub watch
set menuList[3]=all images(littlefs rtos sensor, not including mmcfs)
set menuList[4]=mmcfs(emmc filesystem)
set lastMenuIndex=4

goto :eof

@REM ==================================================
@REM  rtos main
@REM ==================================================

:best1502x_0
setlocal
set programmer=dld\programmer1502x.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% 
if "%BFD_OTA_TYPE%" == "2" (
    set cmd=%cmd% %fwRemapArgs%
) else (
    set cmd=%cmd% %fwArgs%
)
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu

@REM ==================================================
@REM littlefs 
@REM ==================================================

:best1502x_1
setlocal
set programmer=dld\programmer1502x.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %fsArgs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal
goto :showMenu

@REM ==================================================
@REM  sensor hub
@REM ==================================================

:best1502x_2
setlocal
set programmer=dld\programmer1502x.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% -M %sensor%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal
goto :showMenu

@REM ==================================================
@REM  all images
@REM ==================================================

:best1502x_3
setlocal
set programmer=dld\programmer1502x.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% 

if "%BFD_OTA_TYPE%" == "0" (
  set cmd=%cmd% %fwArgs%
) else if "%BFD_OTA_TYPE%" == "1" (
  set cmd=%cmd% %fwArgs% %fwMiniArgs%
) else (
  set cmd=%cmd% %fwRemapArgs%
)
set cmd=%cmd% -M %sensor%

if "%LFS_FLASH_BASE_ADDR%" NEQ "" (
  set cmd=%cmd% %fsArgs%
)

echo SECURE_BOOT=%SECURE_BOOT%
if "%SECURE_BOOT%" == "1" (
  set cmd=%cmd% --sec-file %pub_otp%
)
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal
goto :showMenu

@REM ==================================================
@REM mmcfs
@REM ==================================================

:best1502x_4
setlocal
set programmer=dld\programmer_1502x_emmc.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %mmcArgs%
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu


:best1502x_5
goto :showMenu


:best1502x_6
goto :showMenu




:best1600


:best1600_setMenu
set menuList[0]=rtos
set menuList[1]=rtos with DUAL FLASH CHIP
set menuList[2]=BTH only
set menuList[3]=BTH only with DUAL FLASH CHIP
set menuList[4]=littlefs(norflash fs)
set menuList[5]=littlefs(norflash fs) with DUAL FLASH CHIP
set menuList[6]=all the images(bth, littlefs, rtos)
set menuList[7]=all the images(bth, littlefs, rtos) with DUAL FLASH CHIP.
set menuList[8]=factory
set menuList[9]=mmcfs(emmc fs)

set lastMenuIndex=9

goto :eof

@REM ==================================================
@REM rtos main
@REM ==================================================

:best1600_0
setlocal
set programmer=dld\programmer1600.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% -M %rtos_main%

echo %cmd%
%cmd%
echo %cmd%
pause

endlocal
goto :showMenu

@REM ==================================================
@REM rtos main with DUAL FALSH CHIP
@REM ==================================================

:best1600_1
setlocal
set programmer=dld\programmer1600_dual.bin
set RTOS_OPT=--set-dual-chip 1 --addr 0x2c018000 dld\remap_ctrl_a_noBurnAddr.bin
set REMAP_FILE=dld\ota_boot_remap.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% !REMAP_FILE! !RTOS_OPT! -M  %rtos_main%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu


@REM ==================================================
@REM best1600_watch_bth
@REM ==================================================

:best1600_2
setlocal
set programmer=dld\programmer1600.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% best1600_watch_bth.bin

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu


@REM ==================================================
@REM best1600_watch_bth with DUAL FALSH CHIP
@REM ==================================================

:best1600_3
setlocal
set programmer=dld\programmer1600_dual.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% best1600_watch_bth.bin

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu

@REM ==================================================
@REM littlefs
@REM ==================================================

:best1600_4
setlocal
set programmer=dld\programmer1600.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %fsArgs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu

@REM ==================================================
@REM littlefs with DUAL FLASH CHIP
@REM ==================================================

:best1600_5
setlocal
set programmer=dld\programmer1600_dual.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% --set-dual-chip 1 --addr 0x2c360000 %littlefs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu


@REM ==================================================
@REM all
@REM ==================================================

:best1600_6
setlocal
set programmer=dld\programmer1600.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% --erase-chip %programmer% best1600_watch_bth.bin %rtos_main% %fsArgs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu

@REM ==================================================
@REM all with DUAL FLASH CHIP
@REM ==================================================
:best1600_7
setlocal
set programmer=dld\programmer1600_dual.bin
set RTOS_OPT=--set-dual-chip 1 --addr 0x2c018000 dld\remap_ctrl_a_noBurnAddr.bin
set REMAP_FILE=dld\ota_boot_remap.bin

set cmd=%dldtool% %COM% --reboot %COM_OPT% --erase-chip %programmer% !REMAP_FILE! best1600_watch_bth.bin !RTOS_OPT! -M %rtos_main% %fsArgs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu



@REM ==================================================
@REM factory
@REM ==================================================

:best1600_8
setlocal
set programmer=dld\programmer1600.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %fact2Args%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu


@REM ==================================================
@REM mmcfs
@REM ==================================================
:best1600_9
setlocal
set programmer=dld\programmer_mul_device.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %mmcArgs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu



@REM ==================================================
@REM best2003
@REM ==================================================

:best2003

:best2003_setMenu
set menuList[0]=rtos
set menuList[1]=littlefs(norflash filesystem)
set menuList[2]=factory
set menuList[3]=boot1
set menuList[4]=boot2
set menuList[5]=all images(littlefs rtos sensor)
set lastMenuIndex=5

goto :eof


@REM ==================================================
@REM rtos
@REM ==================================================
:best2003_0
setlocal

set programmer=programmer2003.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer%
if "%BFD_OTA_TYPE%" == "2" (
  set cmd=%cmd% %fwRemapArgs%
) else (
  set cmd=%cmd% %fwArgs%
)

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu



@REM ==================================================
@REM littlefs
@REM ==================================================
:best2003_1
setlocal

set programmer=programmer2003.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %fsArgs%

echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu



@REM ==================================================
@REM factory
@REM ==================================================
:best2003_2
setlocal

set programmer=programmer2003.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %factArgs% %fact2Args%
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu

@REM ==================================================
@REM boot1
@REM ==================================================
:best2003_3
setlocal

set programmer=programmer2003.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %ota_boot1%
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu



@REM ==================================================
@REM boot2
@REM ==================================================
:best2003_4
setlocal

set programmer=programmer2003.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer%
if "%BFD_OTA_TYPE%" == "0" (
  set cmd=%cmd% %boot2Args%
) else if "%BFD_OTA_TYPE%" == "1" (
  set cmd=%cmd% %boot2Args%
) else (
  set cmd=%cmd% %boot2RemapArgs%
)
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu



@REM ==================================================
@REM all
@REM ==================================================
:best2003_5
setlocal

set programmer=programmer2003.bin
set cmd=%dldtool% %COM% --reboot %COM_OPT% %programmer% %ota_boot1%
if "%BFD_OTA_TYPE%" == "0" (
  set cmd=%cmd% %boot2Args% %tzArgs% %fwArgs%
) else if "%BFD_OTA_TYPE%" == "1" (
  set cmd=%cmd% %boot2Args% %tzArgs% %fwArgs% %fwMiniArgs%
) else (
  set cmd=%cmd% %boot2RemapArgs% %tzArgs% %fwRemapArgs%
)

if "%LFS_FLASH_BASE_ADDR%" NEQ "" (
  set cmd=%cmd% %fsArgs%
)
set cmd=%cmd% %factArgs% %fact2Args%
echo SECURE_BOOT=%SECURE_BOOT%
if "%SECURE_BOOT%" == "1" (
  set cmd=%cmd% --sec-file %pub_otp%
)
echo %cmd%
%cmd%
echo %cmd%
pause
endlocal

goto :showMenu

pause

:eof
