@REM start qemu-system-i386  -m 128M -s -S -drive file=disk1.vhd,index=0,media=disk,format=raw -drive file=disk2.vhd,index=1,media=disk,format=raw -serial tcp::23456,server,nowait -d int,pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
@REM start qemu-system-i386  -m 128M -s -S -drive file=disk1.vhd,index=0,media=disk,format=raw -drive file=disk2.vhd,index=1,media=disk,format=raw -serial tcp::23456,server,nowait -d int,pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
start qemu-system-i386 ^
  -m 128M ^
  -s -S ^
  -drive file=disk1.vhd,index=0,media=disk,format=raw ^
  -drive file=disk2.vhd,index=1,media=disk,format=raw ^
  -serial tcp::23456,server,nowait ^
  -d int,pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation ^
  -D E:\ubuntu\diy-x86os\start\start\qemu-debug.log ^
  -no-shutdown ^
  -no-reboot ^
  -no-kvm ^   
