# Diff Details

Date : 2025-06-09 12:53:42

Directory e:\\ubuntu\\diy-x86os\\start\\start\\source

Total : 36 files,  -74458 codes, -124 comments, -400 blanks, all -74982 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [CMakeLists.txt](/CMakeLists.txt) | CMake | -51 | 0 | -13 | -64 |
| [build/.cmake/api/v1/query/client-vscode/query.json](/build/.cmake/api/v1/query/client-vscode/query.json) | JSON | -1 | 0 | 0 | -1 |
| [build/.cmake/api/v1/reply/cache-v2-a9e181b4e40f6141d882.json](/build/.cmake/api/v1/reply/cache-v2-a9e181b4e40f6141d882.json) | JSON | -1,267 | 0 | -1 | -1,268 |
| [build/.cmake/api/v1/reply/cmakeFiles-v1-221b669f9abf26b1a3c6.json](/build/.cmake/api/v1/reply/cmakeFiles-v1-221b669f9abf26b1a3c6.json) | JSON | -469 | 0 | -1 | -470 |
| [build/.cmake/api/v1/reply/codemodel-v2-fb12f78ae4328fd9a472.json](/build/.cmake/api/v1/reply/codemodel-v2-fb12f78ae4328fd9a472.json) | JSON | -159 | 0 | -1 | -160 |
| [build/.cmake/api/v1/reply/directory-.-Debug-d0094a50bb2071803777.json](/build/.cmake/api/v1/reply/directory-.-Debug-d0094a50bb2071803777.json) | JSON | -14 | 0 | -1 | -15 |
| [build/.cmake/api/v1/reply/directory-source.boot-Debug-e32ed8e172d18c41a714.json](/build/.cmake/api/v1/reply/directory-source.boot-Debug-e32ed8e172d18c41a714.json) | JSON | -14 | 0 | -1 | -15 |
| [build/.cmake/api/v1/reply/directory-source.kernel-Debug-4384d93899d7b295e598.json](/build/.cmake/api/v1/reply/directory-source.kernel-Debug-4384d93899d7b295e598.json) | JSON | -14 | 0 | -1 | -15 |
| [build/.cmake/api/v1/reply/directory-source.loader-Debug-c085dfc97c955badf5e0.json](/build/.cmake/api/v1/reply/directory-source.loader-Debug-c085dfc97c955badf5e0.json) | JSON | -14 | 0 | -1 | -15 |
| [build/.cmake/api/v1/reply/index-2025-06-08T08-52-45-0354.json](/build/.cmake/api/v1/reply/index-2025-06-08T08-52-45-0354.json) | JSON | -132 | 0 | -1 | -133 |
| [build/.cmake/api/v1/reply/target-boot-Debug-664a0300e8dbede8f8ae.json](/build/.cmake/api/v1/reply/target-boot-Debug-664a0300e8dbede8f8ae.json) | JSON | -162 | 0 | -1 | -163 |
| [build/.cmake/api/v1/reply/target-kernel-Debug-bb541995888b119b1eb7.json](/build/.cmake/api/v1/reply/target-kernel-Debug-bb541995888b119b1eb7.json) | JSON | -366 | 0 | -1 | -367 |
| [build/.cmake/api/v1/reply/target-loader-Debug-670a68e7d8158d8d51f5.json](/build/.cmake/api/v1/reply/target-loader-Debug-670a68e7d8158d8d51f5.json) | JSON | -170 | 0 | -1 | -171 |
| [build/.cmake/api/v1/reply/toolchains-v1-0345dc3850ce02d89834.json](/build/.cmake/api/v1/reply/toolchains-v1-0345dc3850ce02d89834.json) | JSON | -48 | 0 | -1 | -49 |
| [build/CMakeCache.txt](/build/CMakeCache.txt) | CMake Cache | -312 | 0 | -74 | -386 |
| [build/CMakeFiles/3.23.1/CMakeASMCompiler.cmake](/build/CMakeFiles/3.23.1/CMakeASMCompiler.cmake) | CMake | -14 | 0 | -7 | -21 |
| [build/CMakeFiles/3.23.1/CMakeCCompiler.cmake](/build/CMakeFiles/3.23.1/CMakeCCompiler.cmake) | CMake | -55 | 0 | -18 | -73 |
| [build/CMakeFiles/3.23.1/CMakeSystem.cmake](/build/CMakeFiles/3.23.1/CMakeSystem.cmake) | CMake | -10 | 0 | -6 | -16 |
| [build/CMakeFiles/3.23.1/CompilerIdC/CMakeCCompilerId.c](/build/CMakeFiles/3.23.1/CompilerIdC/CMakeCCompilerId.c) | C | -633 | -61 | -135 | -829 |
| [build/CMakeFiles/CMakeError.log](/build/CMakeFiles/CMakeError.log) | Log | -53 | 0 | -56 | -109 |
| [build/CMakeFiles/CMakeOutput.log](/build/CMakeFiles/CMakeOutput.log) | Log | -20 | 0 | -15 | -35 |
| [build/cmake\_install.cmake](/build/cmake_install.cmake) | CMake | -48 | 0 | -10 | -58 |
| [build/compile\_commands.json](/build/compile_commands.json) | JSON | -112 | 0 | 0 | -112 |
| [build/source/boot/cmake\_install.cmake](/build/source/boot/cmake_install.cmake) | CMake | -33 | 0 | -7 | -40 |
| [build/source/kernel/cmake\_install.cmake](/build/source/kernel/cmake_install.cmake) | CMake | -33 | 0 | -7 | -40 |
| [build/source/loader/cmake\_install.cmake](/build/source/loader/cmake_install.cmake) | CMake | -33 | 0 | -7 | -40 |
| [qemu-debug.log](/qemu-debug.log) | Log | -70,135 | 0 | -1 | -70,136 |
| [script/img-write-linux.sh](/script/img-write-linux.sh) | Shell Script | -16 | -18 | -10 | -44 |
| [script/img-write-osx.sh](/script/img-write-osx.sh) | Shell Script | -16 | -17 | -11 | -44 |
| [script/img-write-win.bat](/script/img-write-win.bat) | Batch | -14 | -25 | -10 | -49 |
| [script/qemu-debug-linux.sh](/script/qemu-debug-linux.sh) | Shell Script | -2 | -1 | -1 | -4 |
| [script/qemu-debug-osx.sh](/script/qemu-debug-osx.sh) | Shell Script | -1 | -1 | -1 | -3 |
| [script/qemu-debug-win.bat](/script/qemu-debug-win.bat) | Batch | -11 | -2 | -1 | -14 |
| [script/test.code-workspace](/script/test.code-workspace) | JSON with Comments | -10 | 0 | 0 | -10 |
| [source/kernel/init/first\_task.c](/source/kernel/init/first_task.c) | C | 7 | 1 | 2 | 10 |
| [start.code-workspace](/start.code-workspace) | JSON with Comments | -23 | 0 | 0 | -23 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details