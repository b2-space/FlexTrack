@echo off
echo ___TTGO T-Beam interactive flashing script___
setlocal enabledelayedexpansion

set "buildPath=..\build\esp32.esp32.t-beam"
set "flashingPath=."
if not exist "%flashingPath%\esptool.exe" (
    echo WARNING: esptool.exe not found in %flashingPath%\.
    pause
    exit /b
)
if not exist "%flashingPath%\boot_app0.bin" (
    echo WARNING: boot_app0.bin not found in %flashingPath%\.
    pause
    exit /b
)
if not exist "%buildPath%\FlexTrack.ino.bootloader.bin" (
    echo WARNING: FlexTrack.ino.bootloader.bin not found in %buildPath%\.
    pause
    exit /b
)
if not exist "%buildPath%\FlexTrack.ino.bin" (
    echo WARNING: FlexTrack.ino.bin not found in %buildPath%\.
    pause
    exit /b
)
if not exist "%buildPath%\FlexTrack.ino.partitions.bin" (
    echo WARNING: FlexTrack.ino.partitions.bin not found in %buildPath%\.
    pause
    exit /b
)

echo Available COM ports:
set "comPortsAvailable=false"
for /f "tokens=2 delims==" %%A in ('wmic path Win32_PnPEntity get caption /format:list 2^>nul ^| find "COM"') do (
    echo %%A
    set "comPortsAvailable=true"
)
if "!comPortsAvailable!"=="false" (
    echo WARNING: No COM ports available.
    pause
    exit /b
)
set /p comPort=Enter the COM port you want to use: 
echo %comPort% | find "COM" >nul
if %errorlevel% neq 0 (
    echo WARNING: Invalid COM port entered.
    pause
    exit /b
)

esptool.exe --chip esp32 --port %comPort% --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0xe000 %flashingPath%\boot_app0.bin 0x1000 %buildPath%\FlexTrack.ino.bootloader.bin 0x10000 %buildPath%\FlexTrack.ino.bin 0x8000 %buildPath%\FlexTrack.ino.partitions.bin
endlocal
