@echo off
setlocal enabledelayedexpansion enableextensions

set file=%1
@REM echo file=%file%

echo Detecting COM ports......

set "count=0"
for /f "tokens=2 delims=COM:" %%i in ('mode ^|findstr /RC:"\C\O\M[0-9*]"') do (
  set comList[!count!]=%%i
  set /a "count+=1"
  @REM set /a count=!count!+1
)
set /a lastIndex=count-1

call :showComs %comList%

@REM ================================================
@REM sort com list
for /l %%i in (0,1,%lastIndex%) do (
  @REM echo process: [%%i]=!comList[%%i]!
  set /a next=%%i+1
  for /l %%j in (!next!,1,%lastIndex%) do (
    @REM echo     process: [%%j]=!comList[%%j]!
    if !comList[%%i]! LEQ !comList[%%j]! (
      @REM echo          less, do nothiing
    ) else (
      @REM echo          greater, need sort
      set /a tv = !comList[%%i]!
      @REM echo          tv=!tv!
      set /a comList[%%i] = !comList[%%j]!
      set /a comList[%%j] = !tv!
    )
  )
)

call :showComs %comList%

:: ================================================
:: list.join(',')
:: ================================================
for /l %%i in (0,1,%lastIndex%) do (
  if "%%i" == "0" (
    set sortedComs=!comList[%%i]!
  ) else (
    set sortedComs=!sortedComs!,!comList[%%i]!
  )
  @REM echo sortedComs: !sortedComs!
)

if "%count%" == "0" (
  echo No COM ports available
  goto :eof
) 
if "%count%" == "1" (
  @REM echo COM=!comList[0]!
  set COM=!comList[0]!
  echo COM=!COM!> %file%
) else (
  @REM echo COMs:
  @REM for /l %%n in (0,1,%lastIndex%) do (
  @REM   echo     [%%n]=!comList[%%n]!
  @REM )


  set /p c="Please input COM port(%sortedComs%)(default !comList[%lastIndex%]!): "
  if "!c!" == "" (
    set COM=!comList[%lastIndex%]!
    echo COM=!comList[%lastIndex%]!> %file%
    goto :eof
    @REM echo You choose no COM port
    @REM exit
  )
  :: check com port input
  for /l %%i in (0,1,%lastIndex%) do (
    if "!comList[%%i]!" == "!c!" (
      echo found [%%i]
      goto :found
    )
  )  
  echo COM!c! not available
  goto :eof
:found
  set COM=!c!
  echo COM=!c!> %file%
  @REM echo You choose COM!c!
)

goto :eof


:showComs
setlocal
  @REM echo COMs:
  @REM for /l %%n in (0,1,%lastIndex%) do (
  @REM   echo     [%%n]=!comList[%%n]!
  @REM )
endlocal
goto :eof