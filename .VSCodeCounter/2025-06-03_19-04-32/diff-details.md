# Diff Details

Date : 2025-06-03 19:04:32

Directory e:\\ubuntu\\diy-x86os\\start\\start

Total : 59 files,  5189 codes, 195 comments, 548 blanks, all 5932 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [CMakeLists.txt](/CMakeLists.txt) | CMake | 51 | 0 | 13 | 64 |
| [build/.cmake/api/v1/query/client-vscode/query.json](/build/.cmake/api/v1/query/client-vscode/query.json) | JSON | 1 | 0 | 0 | 1 |
| [build/.cmake/api/v1/reply/cache-v2-a9e181b4e40f6141d882.json](/build/.cmake/api/v1/reply/cache-v2-a9e181b4e40f6141d882.json) | JSON | 1,267 | 0 | 1 | 1,268 |
| [build/.cmake/api/v1/reply/cmakeFiles-v1-221b669f9abf26b1a3c6.json](/build/.cmake/api/v1/reply/cmakeFiles-v1-221b669f9abf26b1a3c6.json) | JSON | 469 | 0 | 1 | 470 |
| [build/.cmake/api/v1/reply/codemodel-v2-f03cf47790ad7653fd90.json](/build/.cmake/api/v1/reply/codemodel-v2-f03cf47790ad7653fd90.json) | JSON | 159 | 0 | 1 | 160 |
| [build/.cmake/api/v1/reply/directory-.-Debug-d0094a50bb2071803777.json](/build/.cmake/api/v1/reply/directory-.-Debug-d0094a50bb2071803777.json) | JSON | 14 | 0 | 1 | 15 |
| [build/.cmake/api/v1/reply/directory-source.boot-Debug-e32ed8e172d18c41a714.json](/build/.cmake/api/v1/reply/directory-source.boot-Debug-e32ed8e172d18c41a714.json) | JSON | 14 | 0 | 1 | 15 |
| [build/.cmake/api/v1/reply/directory-source.kernel-Debug-4384d93899d7b295e598.json](/build/.cmake/api/v1/reply/directory-source.kernel-Debug-4384d93899d7b295e598.json) | JSON | 14 | 0 | 1 | 15 |
| [build/.cmake/api/v1/reply/directory-source.loader-Debug-c085dfc97c955badf5e0.json](/build/.cmake/api/v1/reply/directory-source.loader-Debug-c085dfc97c955badf5e0.json) | JSON | 14 | 0 | 1 | 15 |
| [build/.cmake/api/v1/reply/index-2025-06-03T07-31-29-0407.json](/build/.cmake/api/v1/reply/index-2025-06-03T07-31-29-0407.json) | JSON | 132 | 0 | 1 | 133 |
| [build/.cmake/api/v1/reply/target-boot-Debug-664a0300e8dbede8f8ae.json](/build/.cmake/api/v1/reply/target-boot-Debug-664a0300e8dbede8f8ae.json) | JSON | 162 | 0 | 1 | 163 |
| [build/.cmake/api/v1/reply/target-kernel-Debug-7865ddca20e6be969b1a.json](/build/.cmake/api/v1/reply/target-kernel-Debug-7865ddca20e6be969b1a.json) | JSON | 322 | 0 | 1 | 323 |
| [build/.cmake/api/v1/reply/target-loader-Debug-670a68e7d8158d8d51f5.json](/build/.cmake/api/v1/reply/target-loader-Debug-670a68e7d8158d8d51f5.json) | JSON | 170 | 0 | 1 | 171 |
| [build/.cmake/api/v1/reply/toolchains-v1-0345dc3850ce02d89834.json](/build/.cmake/api/v1/reply/toolchains-v1-0345dc3850ce02d89834.json) | JSON | 48 | 0 | 1 | 49 |
| [build/CMakeCache.txt](/build/CMakeCache.txt) | CMake Cache | 312 | 0 | 74 | 386 |
| [build/CMakeFiles/3.23.1/CMakeASMCompiler.cmake](/build/CMakeFiles/3.23.1/CMakeASMCompiler.cmake) | CMake | 14 | 0 | 7 | 21 |
| [build/CMakeFiles/3.23.1/CMakeCCompiler.cmake](/build/CMakeFiles/3.23.1/CMakeCCompiler.cmake) | CMake | 55 | 0 | 18 | 73 |
| [build/CMakeFiles/3.23.1/CMakeSystem.cmake](/build/CMakeFiles/3.23.1/CMakeSystem.cmake) | CMake | 10 | 0 | 6 | 16 |
| [build/CMakeFiles/3.23.1/CompilerIdC/CMakeCCompilerId.c](/build/CMakeFiles/3.23.1/CompilerIdC/CMakeCCompilerId.c) | C | 633 | 61 | 135 | 829 |
| [build/CMakeFiles/CMakeError.log](/build/CMakeFiles/CMakeError.log) | Log | 53 | 0 | 56 | 109 |
| [build/CMakeFiles/CMakeOutput.log](/build/CMakeFiles/CMakeOutput.log) | Log | 20 | 0 | 15 | 35 |
| [build/cmake\_install.cmake](/build/cmake_install.cmake) | CMake | 48 | 0 | 10 | 58 |
| [build/compile\_commands.json](/build/compile_commands.json) | JSON | 92 | 0 | 0 | 92 |
| [build/source/boot/cmake\_install.cmake](/build/source/boot/cmake_install.cmake) | CMake | 33 | 0 | 7 | 40 |
| [build/source/kernel/cmake\_install.cmake](/build/source/kernel/cmake_install.cmake) | CMake | 33 | 0 | 7 | 40 |
| [build/source/loader/cmake\_install.cmake](/build/source/loader/cmake_install.cmake) | CMake | 33 | 0 | 7 | 40 |
| [script/img-write-linux.sh](/script/img-write-linux.sh) | Shell Script | 16 | 18 | 10 | 44 |
| [script/img-write-osx.sh](/script/img-write-osx.sh) | Shell Script | 16 | 17 | 11 | 44 |
| [script/img-write-win.bat](/script/img-write-win.bat) | Batch | 14 | 25 | 10 | 49 |
| [script/qemu-debug-linux.sh](/script/qemu-debug-linux.sh) | Shell Script | 2 | 1 | 1 | 4 |
| [script/qemu-debug-osx.sh](/script/qemu-debug-osx.sh) | Shell Script | 1 | 1 | 1 | 3 |
| [script/qemu-debug-win.bat](/script/qemu-debug-win.bat) | Batch | 1 | 1 | 1 | 3 |
| [script/test.code-workspace](/script/test.code-workspace) | JSON with Comments | 10 | 0 | 0 | 10 |
| [source/comm/cpu\_instr.h](/source/comm/cpu_instr.h) | C++ | 14 | 0 | 3 | 17 |
| [source/kernel/core/memory.c](/source/kernel/core/memory.c) | C | 64 | 8 | 10 | 82 |
| [source/kernel/core/task.c](/source/kernel/core/task.c) | C | 231 | 5 | 22 | 258 |
| [source/kernel/cpu/cpu.c](/source/kernel/cpu/cpu.c) | C | 22 | 0 | 2 | 24 |
| [source/kernel/cpu/irq.c](/source/kernel/cpu/irq.c) | C | 22 | 0 | 2 | 24 |
| [source/kernel/dev/time.c](/source/kernel/dev/time.c) | C | 1 | 0 | 1 | 2 |
| [source/kernel/include/core/memory.h](/source/kernel/include/core/memory.h) | C | 19 | 0 | 6 | 25 |
| [source/kernel/include/core/task.h](/source/kernel/include/core/task.h) | C | 64 | 8 | 18 | 90 |
| [source/kernel/include/cpu/cpu.h](/source/kernel/include/cpu/cpu.h) | C++ | 15 | 0 | 2 | 17 |
| [source/kernel/include/cpu/irq.h](/source/kernel/include/cpu/irq.h) | C | 4 | 0 | 2 | 6 |
| [source/kernel/include/ipc/mutex.h](/source/kernel/include/ipc/mutex.h) | C++ | 14 | 0 | 9 | 23 |
| [source/kernel/include/ipc/sem.h](/source/kernel/include/ipc/sem.h) | C | 13 | 0 | 3 | 16 |
| [source/kernel/include/os\_cfg.h](/source/kernel/include/os_cfg.h) | C++ | 0 | 8 | 0 | 8 |
| [source/kernel/include/tools/bitmap.h](/source/kernel/include/tools/bitmap.h) | C++ | 15 | 8 | 6 | 29 |
| [source/kernel/include/tools/klib.h](/source/kernel/include/tools/klib.h) | C++ | 16 | 0 | 4 | 20 |
| [source/kernel/include/tools/list.h](/source/kernel/include/tools/list.h) | C++ | 54 | 0 | 12 | 66 |
| [source/kernel/init/init.c](/source/kernel/init/init.c) | C | 54 | 18 | 7 | 79 |
| [source/kernel/init/start.S](/source/kernel/init/start.S) | x86 and x86_64 Assembly | 61 | 0 | 22 | 83 |
| [source/kernel/ipc/mutex.c](/source/kernel/ipc/mutex.c) | C | 49 | 8 | 6 | 63 |
| [source/kernel/ipc/sem.c](/source/kernel/ipc/sem.c) | C | 47 | 0 | 4 | 51 |
| [source/kernel/kernel.lds](/source/kernel/kernel.lds) | LinkerScript | 1 | 0 | 1 | 2 |
| [source/kernel/tools/bitmap.c](/source/kernel/tools/bitmap.c) | C | 65 | 0 | 2 | 67 |
| [source/kernel/tools/klib.c](/source/kernel/tools/klib.c) | C | 11 | 0 | -1 | 10 |
| [source/kernel/tools/list.c](/source/kernel/tools/list.c) | C | 78 | 0 | 6 | 84 |
| [source/kernel/tools/log.c](/source/kernel/tools/log.c) | C | 6 | 8 | -2 | 12 |
| [start.code-workspace](/start.code-workspace) | JSON with Comments | 16 | 0 | 0 | 16 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details