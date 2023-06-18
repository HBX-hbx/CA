# Tomasulo 模拟器 && 缓存测量实验

指令集使用 RISC-V 32 IM

实验文档在 [这里](https://lab.cs.tsinghua.edu.cn/ca-lab-docs/labs/tomasulo/) 发布，可以查阅实验要求和实验指导书。

有任何疑问，请在 **课程交流群 / 网络学堂答疑区** 联系助教。

## 目录结构

```
.
├── backend             # 后端文件
├── checkfiles          # 测例文件
├── CMakeLists.txt
├── common              # 通用源码
├── frontend            # 前端文件
├── include             # 头文件
├── program             # 可执行程序
├── readme.md 
├── test                # 测试用户程序
└── thirdparty          # 第三方代码
└── lab2-cache          # 缓存测量实验
```

## 注意事项

1. ICache 地址映射至 `0x80000000 ~ 0x803FFFFF`，DCache 地址映射至 `0x80400000 ~ 0x807FFFFF`
2. 已经初始化 sp 为 0x80800000，gp 为 0x80400000
3. 目前使用 `0x0000000b` 作为 exit mark

## 实验准备

实验之前，需要准备好如下工具：

1. cmake，g++: cmake 最低版本为 3.15，g++ 最低版本为 9.0.0
2. （可选） riscv 工具链：riscv-gnu-linux- 或 riscv-unknown-elf- 工具链均可。推荐使用 linux 环境，工具链可以通过 apt 直接安装。
    
    ```bash
    sudo apt install g++-riscv64-linux-gnu
    sudo apt install g++-riscv64-unknown-elf
    ```

3. 如果没有工具链，则需要准备好预先下发的测例 elf 文件用于测试

## 使用方法

该实验框架使用 cmake 编译，编译方法如下：

```bash
mkdir build
cd build
cmake ..
make
```

如果使用 riscv64-linux-gnu-toolchain，则在编译前修改 `./test/CMakeLists.txt` 中的 Toolchain prefix 

`test` 文件夹用于存放测例，如果增加了新的测例，请手动重新 cmake。

如果没有工具链，则请注释掉 `./CMakeLists.txt` 的最后一行，即可不编译测例文件。

之后，可以运行 checker 检查实现的正确性，如：

对 riscv64-unknown-elf-toolchain：

```bash
./checker -f ./test/selection_sort -c ../checkfiles/selection_sort.chk 
```

对 riscv64-linux-gnu-toolchain:

```bash
./checker -f ./test/selection_sort -c ../checkfiles/selection_sort_linux.chk
```

若最后一行显示：`[   OK    ] 16 testcase(s) passed`，则说明当前测例通过，可以继续测试其他测例。

