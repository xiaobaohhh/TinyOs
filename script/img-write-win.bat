@REM @echo off
@REM REM 检查并请求管理员权限
@REM net session >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM     echo 脚本需要管理员权限，正在请求...
@REM     powershell -Command "Start-Process cmd -ArgumentList '/c \"%~f0\"' -Verb runAs"
@REM     exit /b
@REM )

if not exist "disk1.vhd" (
    echo "disk1.vhd not found in image directory"
    notepad win_error.txt
    exit -1
)

if not exist "disk2.vhd" (
    echo "disk2.vhd not found in image directory"
    notepad win_error.txt
    exit -1
)

set DISK1_NAME=disk1.vhd

dd if=boot.bin of=%DISK1_NAME% bs=512 conv=notrunc count=1

dd if=loader.bin of=%DISK1_NAME% bs=512 conv=notrunc seek=1

dd if=kernel.elf of=%DISK1_NAME% bs=512 conv=notrunc seek=100

@REM dd if=init.elf of=%DISK1_NAME% bs=512 conv=notrunc seek=5000
dd if=shell.elf of=%DISK1_NAME% bs=512 conv=notrunc seek=5000

set DISK2_NAME=disk2.vhd
set TARGET_PATH=k
echo select vdisk file="%cd%\%DISK2_NAME%" >a.txt
echo attach vdisk >>a.txt
echo select partition 1 >> a.txt
echo assign letter=%TARGET_PATH% >> a.txt
diskpart /s a.txt
if %errorlevel% neq 0 (
    echo "attach disk2 failed"
    exit -1
)
del a.txt

copy /Y init.elf %TARGET_PATH%:\init
copy /Y shell.elf %TARGET_PATH%:\shell.elf
copy /Y loop.elf %TARGET_PATH%:\loop

echo select vdisk file="%cd%\%DISK2_NAME%" >a.txt
echo detach vdisk >>a.txt
diskpart /s a.txt
if %errorlevel% neq 0 (
    echo "attach disk2 failed"
    exit -1
)
del a.txt
