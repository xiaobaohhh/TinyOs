# Details

Date : 2025-06-09 12:55:34

Directory e:\\ubuntu\\diy-x86os\\start\\start\\source

Total : 47 files,  3118 codes, 237 comments, 501 blanks, all 3856 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [source/applib/lib\_syscall.h](/source/applib/lib_syscall.h) | C | 61 | 8 | 7 | 76 |
| [source/boot/CMakeLists.txt](/source/boot/CMakeLists.txt) | CMake | 15 | 0 | 4 | 19 |
| [source/boot/boot.c](/source/boot/boot.c) | C | 6 | 14 | 4 | 24 |
| [source/boot/boot.h](/source/boot/boot.h) | C++ | 3 | 10 | 2 | 15 |
| [source/boot/start.S](/source/boot/start.S) | x86 and x86_64 Assembly | 40 | 0 | 6 | 46 |
| [source/comm/boot\_info.h](/source/comm/boot_info.h) | C++ | 14 | 0 | 7 | 21 |
| [source/comm/cpu\_instr.h](/source/comm/cpu_instr.h) | C++ | 115 | 0 | 15 | 130 |
| [source/comm/elf.h](/source/comm/elf.h) | C++ | 43 | 2 | 13 | 58 |
| [source/comm/types.h](/source/comm/types.h) | C++ | 7 | 0 | 2 | 9 |
| [source/kernel/CMakeLists.txt](/source/kernel/CMakeLists.txt) | CMake | 15 | 0 | 3 | 18 |
| [source/kernel/core/memory.c](/source/kernel/core/memory.c) | C | 236 | 25 | 39 | 300 |
| [source/kernel/core/syscall.c](/source/kernel/core/syscall.c) | C | 30 | 0 | 2 | 32 |
| [source/kernel/core/task.c](/source/kernel/core/task.c) | C | 411 | 11 | 45 | 467 |
| [source/kernel/cpu/cpu.c](/source/kernel/cpu/cpu.c) | C | 78 | 13 | 9 | 100 |
| [source/kernel/cpu/irq.c](/source/kernel/cpu/irq.c) | C | 289 | 1 | 20 | 310 |
| [source/kernel/cpu/mmu.c](/source/kernel/cpu/mmu.c) | C | 0 | 8 | 1 | 9 |
| [source/kernel/dev/time.c](/source/kernel/dev/time.c) | C | 26 | 8 | 4 | 38 |
| [source/kernel/include/core/memory.h](/source/kernel/include/core/memory.h) | C | 34 | 8 | 7 | 49 |
| [source/kernel/include/core/syscall.h](/source/kernel/include/core/syscall.h) | C | 34 | 0 | 1 | 35 |
| [source/kernel/include/core/task.h](/source/kernel/include/core/task.h) | C | 75 | 8 | 25 | 108 |
| [source/kernel/include/cpu/cpu.h](/source/kernel/include/cpu/cpu.h) | C++ | 60 | 8 | 15 | 83 |
| [source/kernel/include/cpu/irq.h](/source/kernel/include/cpu/irq.h) | C | 87 | 0 | 22 | 109 |
| [source/kernel/include/cpu/mmu.h](/source/kernel/include/cpu/mmu.h) | C | 71 | 0 | 8 | 79 |
| [source/kernel/include/dev/time.h](/source/kernel/include/dev/time.h) | C++ | 11 | 0 | 5 | 16 |
| [source/kernel/include/ipc/mutex.h](/source/kernel/include/ipc/mutex.h) | C | 14 | 0 | 9 | 23 |
| [source/kernel/include/ipc/sem.h](/source/kernel/include/ipc/sem.h) | C | 13 | 0 | 3 | 16 |
| [source/kernel/include/os\_cfg.h](/source/kernel/include/os_cfg.h) | C | 13 | 8 | 8 | 29 |
| [source/kernel/include/tools/bitmap.h](/source/kernel/include/tools/bitmap.h) | C | 15 | 8 | 6 | 29 |
| [source/kernel/include/tools/klib.h](/source/kernel/include/tools/klib.h) | C | 29 | 0 | 8 | 37 |
| [source/kernel/include/tools/list.h](/source/kernel/include/tools/list.h) | C++ | 54 | 0 | 12 | 66 |
| [source/kernel/include/tools/log.h](/source/kernel/include/tools/log.h) | C | 5 | 0 | 5 | 10 |
| [source/kernel/init/first\_task.c](/source/kernel/init/first_task.c) | C | 46 | 23 | 5 | 74 |
| [source/kernel/init/first\_task\_entry.S](/source/kernel/init/first_task_entry.S) | x86 and x86_64 Assembly | 10 | 0 | 1 | 11 |
| [source/kernel/init/init.c](/source/kernel/init/init.c) | C | 46 | 52 | 9 | 107 |
| [source/kernel/init/init.h](/source/kernel/init/init.h) | C++ | 3 | 0 | 5 | 8 |
| [source/kernel/init/start.S](/source/kernel/init/start.S) | x86 and x86_64 Assembly | 394 | 0 | 88 | 482 |
| [source/kernel/ipc/mutex.c](/source/kernel/ipc/mutex.c) | C | 49 | 8 | 6 | 63 |
| [source/kernel/ipc/sem.c](/source/kernel/ipc/sem.c) | C | 47 | 0 | 4 | 51 |
| [source/kernel/tools/bitmap.c](/source/kernel/tools/bitmap.c) | C | 65 | 0 | 2 | 67 |
| [source/kernel/tools/klib.c](/source/kernel/tools/klib.c) | C | 219 | 3 | 12 | 234 |
| [source/kernel/tools/list.c](/source/kernel/tools/list.c) | C | 78 | 0 | 6 | 84 |
| [source/kernel/tools/log.c](/source/kernel/tools/log.c) | C | 38 | 11 | 10 | 59 |
| [source/loader/CMakeLists.txt](/source/loader/CMakeLists.txt) | CMake | 15 | 0 | 3 | 18 |
| [source/loader/loader.h](/source/loader/loader.h) | C++ | 17 | 0 | 8 | 25 |
| [source/loader/loader\_16.c](/source/loader/loader_16.c) | C | 67 | 0 | 13 | 80 |
| [source/loader/loader\_32.c](/source/loader/loader_32.c) | C | 91 | 0 | 6 | 97 |
| [source/loader/start.S](/source/loader/start.S) | x86 and x86_64 Assembly | 29 | 0 | 6 | 35 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)