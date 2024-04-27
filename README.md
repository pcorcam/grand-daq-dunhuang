# GRAND-DAQ使用说明

> version: 0.1
> contacts: gumh@ihep.ac.cn, duanbh@ihep.ac.cn

### 功能

已实现功能：
- 电子学数据读出
- 电子学原始数据直接存盘
- 支持多个DU

注意：
- 当前版本没有T3触发
- 多个DU采集数据时，事例写盘并不是严格按照事例产生的时间顺序进行的

### 配置

配置环境变量: grand-daq/env.sh
```
GRAND_DAQ_HOME=<grand-daq和external文件夹所在目录>
```

### 运行CSDAQ

```
csdaq_run
```

### 运行配置文件

- 电子学配置文件：grand-daq/cfgs/DU-readable-conf.yaml
- 日志等级配置文件：grand-daq/cfgs/logging/*

