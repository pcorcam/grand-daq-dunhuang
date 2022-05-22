# 安装使用说明

## 软件编译

1. 修改CMakeList.txt文件，配置external依赖软件所在路径

对于PMO测试计算机，变量设置如下：
```
set(EXTERNAL_DIR_ARM /home/grand/arm_a_installed)
set(EXTERNAL_DIR_PC /home/grand/pc_installed)
```

对于高能所开发环境，变量设置如下：
```
set(EXTERNAL_DIR_PC /data/gv-dev/sys/grand/pc_installed)

> 高能所环境没有ARM平台交叉编译，只能编译DUMMY DU插件。

2. 修改build.sh

对于PMO测试计算机，变量设置如下：
```
CMAKE=cmake
```

对于高能所开发环境，变量设置如下：
```
CMAKE=cmake3
```

3. 开始编译

3.1 配置环境：
```
source env.sh
```

3.2 编译ARM环境所需的软件（DU-DAQ）：
```
./build.sh arm
```

3.3 编译计算机端软件（DU-DAQ with dummy plugin & CS-DAQ）
```
./build.sh pc
```

> NOTE:
> 编译完成后软件安装在`./installed`目录下。

## 运行


## 软件打包

## 打包后运行
