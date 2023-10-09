@echo off

rem Usage: burn.bat [all]

set cwd=%~dp0

echo > %cwd%.com
call %cwd%select_com.bat %cwd%.com

for /f "tokens=1,2 delims==" %%i in ('type %cwd%.com') do (
  set %%i=%%j
  if %%i == "COM" ( goto :next)
)
:next

TITLE Burning %1 @COM=%COM%
if "%COM%" == "" ( pause && goto :eof )
echo COM=%COM%

set dldtool=%cwd%burn\dldtool.exe
@REM if NOT EXIST %dldtool% (
@REM   set dldtool=%cwd%tool\dldtool.exe
@REM )
echo Using %dldtool%
@REM set buildinfo=%cwd%..\out\best2600w_aiot\.buildinfo
@REM if NOT EXIST %buildinfo% (
  set buildinfo=%cwd%.buildinfo
@REM )

if not exist %buildinfo% (
  echo %buildinfo% not exists, please run tools/build.sh
  pause
  exit
)

set progRate=1500000
rem set progRate=3000000

@echo Read buildinfo...
@echo =====================================
for /f "tokens=1,2 delims==" %%i in (%buildinfo%) do (
  echo     %%i=%%j
  set %%i=%%j
)
@echo =====================================

rem @echo FLASH_TYPE=%FLASH_TYPE%
cd %cwd%

set programmer=ota_bin\programmer2003.bin
set binPath=ota_bin
set littlefs=%binPath%\littlefs.bin
set ota_boot1=%binPath%\ota_boot1.bin
set ota_boot2a=%binPath%\ota_boot2a.bin
set ota_rtos=%binPath%\ota_rtos.bin
set factory=%binPath%\factory.bin
set pub_otp=%binPath%\pub_otp.bin

echo ota_boot1=%ota_boot1%

@echo Using programmer: %programmer%

set /a fwAddr=FLASH_BASE + OTA_CODE_OFFSET
echo fwAddr=%fwAddr%


set /a boot2Addr=%FLASH_BASE% + %BOOT1_SIZE%
set /a fsaddr=%FLASH_BASE% + %LFS_FLASH_BASE_ADDR%
@REM set /a factoryAddr=%FLASH_BASE% + %RESERVED_SECTION_OFFSET%
@REM set /a factory2Addr=%FLASH_BASE% + %FACTORY_SECTION_OFFSET%
set /a factoryAddr=%FLASH_BASE% + %FACTORY_SECTION_OFFSET% + %FACTORY_SECTOR_OFFSET%
set /a factory2Addr=%FLASH_BASE% + %FACTORY_SECTION_OFFSET% + %FACTORY_SECTION_SIZE%/2 + %FACTORY_SECTOR_OFFSET%
echo boot2Addr=%boot2Addr%
echo fsaddr=%fsaddr%
echo factoryAddr=%factoryAddr%

set cmd=%dldtool% %COM% --reboot --pgm-rate %progRate% %programmer% 

set cmdAll=%cmd% %ota_boot1%
set cmdAll=%cmdAll% --addr %boot2Addr% --remap-both %BOOT2_SIZE% %ota_boot2a% -m %boot2Addr%
set cmdAll=%cmdAll% --addr %fwAddr% --remap-both %OTA_CODE_SIZE% %ota_rtos% -m %fwAddr%
set cmdAll=%cmdAll% --addr %fsaddr% %littlefs%
@REM set cmdAll=%cmdAll% --addr %factoryAddr% --remap-both %FACTORY_SECTION_SIZE% %factory%
set cmdAll=%cmdAll% --addr %factoryAddr% %factory%
set cmdAll=%cmdAll% --addr %factory2Addr% %factory%
rem set cmdAll=%cmdAll% --addr %factory2Addr% %factory%

if "%1" == "all" (
  rem @echo FLASH_BASE=%FLASH_BASE%
  rem @echo FLASH_SIZE=%FLASH_SIZE%
  rem @echo LFS_FLASH_BASE_ADDR=%LFS_FLASH_BASE_ADDR%
  rem @echo FACTORY_SECTION_SIZE=%FACTORY_SECTION_SIZE%

  echo SECURE_BOOT=%SECURE_BOOT%
  if "%SECURE_BOOT%" == "1" (
    echo %cmdAll% --sec-file %pub_otp%
    %cmdAll% --sec-file %pub_otp%
  ) else (
    echo %cmdAll%
    %cmdAll%
  )
) else (
  echo %cmd% --addr %fwAddr% --remap-both %OTA_CODE_SIZE% %ota_rtos% -m %fwAddr%
  %cmd% --addr %fwAddr% --remap-both %OTA_CODE_SIZE% %ota_rtos% -m %fwAddr%
  @REM echo %cmd% --addr %fwAddr% %ota_rtos% -m %fwAddr%
  @REM %cmd% --addr %fwAddr% %ota_rtos% -m %fwAddr%
)

pause