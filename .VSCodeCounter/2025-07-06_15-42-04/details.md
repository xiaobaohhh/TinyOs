# Details

Date : 2025-07-06 15:42:04

Directory e:\\ubuntu\\diy-x86os\\start\\start\\source

Total : 79 files,  7412 codes, 454 comments, 1006 blanks, all 8872 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [start/source/applib/CMakeLists.txt](/start/source/applib/CMakeLists.txt) | CMake | 9 | 0 | 3 | 12 |
| [start/source/applib/crt0.S](/start/source/applib/crt0.S) | x86 and x86_64 Assembly | 10 | 0 | 1 | 11 |
| [start/source/applib/cstart.c](/start/source/applib/cstart.c) | C | 16 | 8 | 2 | 26 |
| [start/source/applib/lib\_syscall.c](/start/source/applib/lib_syscall.c) | C | 208 | 0 | 17 | 225 |
| [start/source/applib/lib\_syscall.h](/start/source/applib/lib_syscall.h) | C | 49 | 8 | 16 | 73 |
| [start/source/boot/CMakeLists.txt](/start/source/boot/CMakeLists.txt) | CMake | 15 | 0 | 4 | 19 |
| [start/source/boot/boot.c](/start/source/boot/boot.c) | C | 6 | 14 | 4 | 24 |
| [start/source/boot/boot.h](/start/source/boot/boot.h) | C++ | 3 | 10 | 2 | 15 |
| [start/source/boot/start.S](/start/source/boot/start.S) | x86 and x86_64 Assembly | 40 | 0 | 6 | 46 |
| [start/source/comm/boot\_info.h](/start/source/comm/boot_info.h) | C | 14 | 0 | 7 | 21 |
| [start/source/comm/cpu\_instr.h](/start/source/comm/cpu_instr.h) | C | 119 | 0 | 18 | 137 |
| [start/source/comm/elf.h](/start/source/comm/elf.h) | C++ | 43 | 2 | 13 | 58 |
| [start/source/comm/types.h](/start/source/comm/types.h) | C | 7 | 0 | 2 | 9 |
| [start/source/init/CMakeLists.txt](/start/source/init/CMakeLists.txt) | CMake | 20 | 0 | 5 | 25 |
| [start/source/init/main.c](/start/source/init/main.c) | C | 15 | 8 | 4 | 27 |
| [start/source/init/main.h](/start/source/init/main.h) | C | 23 | 8 | 12 | 43 |
| [start/source/kernel/CMakeLists.txt](/start/source/kernel/CMakeLists.txt) | CMake | 16 | 0 | 3 | 19 |
| [start/source/kernel/core/memory.c](/start/source/kernel/core/memory.c) | C | 315 | 39 | 56 | 410 |
| [start/source/kernel/core/syscall.c](/start/source/kernel/core/syscall.c) | C | 54 | 0 | 11 | 65 |
| [start/source/kernel/core/task.c](/start/source/kernel/core/task.c) | C | 740 | 44 | 75 | 859 |
| [start/source/kernel/cpu/cpu.c](/start/source/kernel/cpu/cpu.c) | C | 78 | 13 | 9 | 100 |
| [start/source/kernel/cpu/irq.c](/start/source/kernel/cpu/irq.c) | C | 311 | 1 | 21 | 333 |
| [start/source/kernel/cpu/mmu.c](/start/source/kernel/cpu/mmu.c) | C | 0 | 8 | 1 | 9 |
| [start/source/kernel/dev/console.c](/start/source/kernel/dev/console.c) | C | 345 | 23 | 27 | 395 |
| [start/source/kernel/dev/dev.c](/start/source/kernel/dev/dev.c) | C | 93 | 5 | 17 | 115 |
| [start/source/kernel/dev/disk.c](/start/source/kernel/dev/disk.c) | C | 238 | 6 | 21 | 265 |
| [start/source/kernel/dev/kbd.c](/start/source/kernel/dev/kbd.c) | C | 211 | 23 | 12 | 246 |
| [start/source/kernel/dev/time.c](/start/source/kernel/dev/time.c) | C | 26 | 8 | 4 | 38 |
| [start/source/kernel/dev/tty.c](/start/source/kernel/dev/tty.c) | C | 206 | 8 | 16 | 230 |
| [start/source/kernel/fs/devfs/devfs.c](/start/source/kernel/fs/devfs/devfs.c) | C | 85 | 0 | 5 | 90 |
| [start/source/kernel/fs/fatfs/fatfs.c](/start/source/kernel/fs/fatfs/fatfs.c) | C | 628 | 25 | 69 | 722 |
| [start/source/kernel/fs/file.c](/start/source/kernel/fs/file.c) | C | 41 | 8 | 9 | 58 |
| [start/source/kernel/fs/fs.c](/start/source/kernel/fs/fs.c) | C | 404 | 8 | 29 | 441 |
| [start/source/kernel/include/core/memory.h](/start/source/kernel/include/core/memory.h) | C | 42 | 8 | 8 | 58 |
| [start/source/kernel/include/core/syscall.h](/start/source/kernel/include/core/syscall.h) | C++ | 52 | 8 | 8 | 68 |
| [start/source/kernel/include/core/task.h](/start/source/kernel/include/core/task.h) | C | 97 | 8 | 27 | 132 |
| [start/source/kernel/include/cpu/cpu.h](/start/source/kernel/include/cpu/cpu.h) | C++ | 60 | 8 | 15 | 83 |
| [start/source/kernel/include/cpu/irq.h](/start/source/kernel/include/cpu/irq.h) | C++ | 90 | 0 | 23 | 113 |
| [start/source/kernel/include/cpu/mmu.h](/start/source/kernel/include/cpu/mmu.h) | C++ | 71 | 0 | 8 | 79 |
| [start/source/kernel/include/dev/console.h](/start/source/kernel/include/dev/console.h) | C++ | 86 | 0 | 11 | 97 |
| [start/source/kernel/include/dev/dev.h](/start/source/kernel/include/dev/dev.h) | C++ | 35 | 0 | 7 | 42 |
| [start/source/kernel/include/dev/disk.h](/start/source/kernel/include/dev/disk.h) | C | 83 | 0 | 21 | 104 |
| [start/source/kernel/include/dev/kbd.h](/start/source/kernel/include/dev/kbd.h) | C++ | 58 | 0 | 13 | 71 |
| [start/source/kernel/include/dev/time.h](/start/source/kernel/include/dev/time.h) | C++ | 11 | 0 | 5 | 16 |
| [start/source/kernel/include/dev/tty.h](/start/source/kernel/include/dev/tty.h) | C++ | 37 | 8 | 6 | 51 |
| [start/source/kernel/include/fs/devfs/devfs.h](/start/source/kernel/include/fs/devfs/devfs.h) | C++ | 11 | 0 | 4 | 15 |
| [start/source/kernel/include/fs/fatfs/fatfs.h](/start/source/kernel/include/fs/fatfs/fatfs.h) | C++ | 72 | 3 | 16 | 91 |
| [start/source/kernel/include/fs/file.h](/start/source/kernel/include/fs/file.h) | C | 32 | 8 | 8 | 48 |
| [start/source/kernel/include/fs/fs.h](/start/source/kernel/include/fs/fs.h) | C | 62 | 0 | 9 | 71 |
| [start/source/kernel/include/ipc/mutex.h](/start/source/kernel/include/ipc/mutex.h) | C++ | 14 | 0 | 9 | 23 |
| [start/source/kernel/include/ipc/sem.h](/start/source/kernel/include/ipc/sem.h) | C++ | 13 | 0 | 3 | 16 |
| [start/source/kernel/include/os\_cfg.h](/start/source/kernel/include/os_cfg.h) | C++ | 14 | 8 | 9 | 31 |
| [start/source/kernel/include/tools/bitmap.h](/start/source/kernel/include/tools/bitmap.h) | C++ | 15 | 8 | 6 | 29 |
| [start/source/kernel/include/tools/klib.h](/start/source/kernel/include/tools/klib.h) | C | 31 | 8 | 7 | 46 |
| [start/source/kernel/include/tools/list.h](/start/source/kernel/include/tools/list.h) | C++ | 54 | 0 | 12 | 66 |
| [start/source/kernel/include/tools/log.h](/start/source/kernel/include/tools/log.h) | C | 5 | 0 | 5 | 10 |
| [start/source/kernel/init/first\_task.c](/start/source/kernel/init/first_task.c) | C | 62 | 14 | 3 | 79 |
| [start/source/kernel/init/first\_task\_entry.S](/start/source/kernel/init/first_task_entry.S) | x86 and x86_64 Assembly | 10 | 0 | 1 | 11 |
| [start/source/kernel/init/init.c](/start/source/kernel/init/init.c) | C | 57 | 14 | 12 | 83 |
| [start/source/kernel/init/init.h](/start/source/kernel/init/init.h) | C++ | 3 | 0 | 5 | 8 |
| [start/source/kernel/init/lib\_syscall.c](/start/source/kernel/init/lib_syscall.c) | C | 150 | 0 | 12 | 162 |
| [start/source/kernel/init/start.S](/start/source/kernel/init/start.S) | x86 and x86_64 Assembly | 449 | 0 | 102 | 551 |
| [start/source/kernel/ipc/mutex.c](/start/source/kernel/ipc/mutex.c) | C | 49 | 8 | 6 | 63 |
| [start/source/kernel/ipc/sem.c](/start/source/kernel/ipc/sem.c) | C | 47 | 0 | 4 | 51 |
| [start/source/kernel/tools/bitmap.c](/start/source/kernel/tools/bitmap.c) | C | 65 | 0 | 2 | 67 |
| [start/source/kernel/tools/klib.c](/start/source/kernel/tools/klib.c) | C | 244 | 3 | 14 | 261 |
| [start/source/kernel/tools/list.c](/start/source/kernel/tools/list.c) | C | 78 | 8 | 6 | 92 |
| [start/source/kernel/tools/log.c](/start/source/kernel/tools/log.c) | C | 49 | 13 | 12 | 74 |
| [start/source/loader/CMakeLists.txt](/start/source/loader/CMakeLists.txt) | CMake | 15 | 0 | 3 | 18 |
| [start/source/loader/loader.h](/start/source/loader/loader.h) | C++ | 17 | 0 | 8 | 25 |
| [start/source/loader/loader\_16.c](/start/source/loader/loader_16.c) | C | 66 | 9 | 13 | 88 |
| [start/source/loader/loader\_32.c](/start/source/loader/loader_32.c) | C | 91 | 0 | 6 | 97 |
| [start/source/loader/start.S](/start/source/loader/start.S) | x86 and x86_64 Assembly | 29 | 0 | 6 | 35 |
| [start/source/loop/CMakeLists.txt](/start/source/loop/CMakeLists.txt) | CMake | 20 | 0 | 5 | 25 |
| [start/source/loop/main.c](/start/source/loop/main.c) | C | 58 | 8 | 4 | 70 |
| [start/source/loop/main.h](/start/source/loop/main.h) | C | 23 | 8 | 12 | 43 |
| [start/source/shell/CMakeLists.txt](/start/source/shell/CMakeLists.txt) | CMake | 20 | 0 | 5 | 25 |
| [start/source/shell/main.c](/start/source/shell/main.c) | C | 354 | 9 | 12 | 375 |
| [start/source/shell/main.h](/start/source/shell/main.h) | C | 23 | 8 | 12 | 43 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)