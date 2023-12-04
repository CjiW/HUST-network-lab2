# RDT 协议设计实验

## 目录结构
```
.
├── bin
│   ├── gbn
│   ├── sr
│   ├── stop_wait
│   └── tcp
├── include/
├── lib
│   └── libnetsim.a
├── src_gbn/
├── src_sr/
├── src_stopwait/
├── src_tcp/
├── README.md
├── CMakeLists.txt
├── input.txt
├── output.txt
└── test.sh
```

## 编译方法

1. 创建并进入 `build` 文件夹
```shell
(project_dir) mkdir build && cd build
```
2. 构建，目标位置为 `项目目录/bin`
```shell
(project_dir/build) cmake ..
(project_dir/build) make
```
## 运行方法

命令格式为 `可执行文件路径 输入文件路径 输出文件路径`，仿真系统日志将输出到标准输出，收发窗口信息将输出到 `gbn_log.txt` 文件。
```shell
(project_dir/build) ../bin/gbn ../input.txt ../output.txt
or
(project_dir) ./bin/gbn input.txt output.txt
or
(project_dir/bin) ./gbn ../input.txt ../output.txt
```

## 测试脚本

脚本将重复运行十次，并且使用 `diff` 工具比较文件内容，并且生成一个 `test.log` 文件

```shell
(project_dir) ./test.sh gbn
```