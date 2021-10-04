# 功能组件

## protobuf

使用protobuf进行数据结构的一致性，进行编译后可以形成指定文件的源代码，在c++中是name.pb.h，使用的时候直接按类来用就可以。

planning模块的很多逻辑走向是由配置文件决定的（通过将这部分内容从代码中剥离，可以方便的直接对配置文件进行调整，而不用编译源代码），conf文件夹统一存放配置文件，文件名后缀为pb.txt，和proto中的proto文件相对应，可以直接被proto文件生成的数据结构读取。**也就是proto定义了配置的field，conf指定了field的值**

## gflag

**使用gflag库，进行参数设置。**可以运行时从命令行改变参数，如果在命令行没有给出该参数的值则使用定义的默认值。一些文件路径等值可以只用这个库

# DreamView模块启动过程

启动脚本文件：**scripts/bootstrap.**

# planning模块架构

[参考1](https://blog.csdn.net/davidhopper/article/details/89360385)

[参考2](https://blog.csdn.net/davidhopper/article/details/89360385)

## Planning模块启动过程分析

首先，planning模块里是没有main函数的，**apollo各个模块是从dreamview模块启动的**。

HMI::RegisterMessageHandlers()为dreamview的消息响应函数，在dreamview中点击的后台逻辑在这里。

然后调用Trigger（HMIAction::START_MODULE,"Planning")，在Trigger()中调用StartModule("Planning")。

在StartModule中调用System(module_conf->start_command())以执行命令行命令

```sh
nohup mainboard -p compute_sched -d /apollo/modules/planning/dag/planning.dag &
```

在这里，**HMIWorker::config_ 存放着配置文件名和配置文件路径的映射，在LoadConfig()函数中读取了config/hmi_modes/下的配置文件名和路径，各个文件对应了dreamview的不同模式。**

**HMIWorker::current_mode_存放当前模式的各种配置项，在LoadMode()中根据相应模式读取config_里的配置文件，这里面就包括各个模块的启动命令**

main函数蕴含在cyber/manboard/manboard.cc文件中，**其主要过程就是调用ModuleController::LoadModule()函数加载libplanning_component.so以及创建PlanningComponent类。**

