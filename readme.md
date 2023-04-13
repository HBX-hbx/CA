# Tomasulo 模拟器

2023 计算机系统结构 Tomasulo 模拟器实验框架

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

所有需要实现 / 修改的地方已经使用 `TODO:` 标出，可以全局搜索进行定位。

请不要修改任何没有使用 `TODO:` 标记出的文件，这可能会导致你无法通过后续测试。

## 关于 Gitlab CI **（！重要！）**

正常情况下，当你执行 `git push`将本地仓库push到远端后，gitlab即会将你的评测任务加入到执行队列中。

为了保证 `Gitlab CI` 的正常运行，**请不要修改 `.gitlab-ci.yml` ，请不要修改 `.gitlab-ci.yml` ，请不要修改 `.gitlab-ci.yml` 。**

### CI任务说明

运行的任务包含两个，分别为 `job 1` 和 `job 2` 。 `job 1` 对应Tomasulo调度的测试（不包括分支预测）。 `job 2` 对应包含了分支预测的测试。

针对完成或者没有完成分支预测选作内容，你应该对仓库内的文件进行如下的修改。

#### 我完成了Tomasulo后端，不包括分支预测
请将 `check_predict.sh` 文件中第一行的 `export CHECK=true` 改为 `export CHECK=false` 。

#### 我完成了选作内容（分支预测）
请确保 `check_predict.sh` 文件中第一行为 `export CHECK=true` 。

