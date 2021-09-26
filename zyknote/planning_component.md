# 功能组件

## protobuf

使用protobuf进行数据结构的一致性，进行编译后可以形成指定文件的源代码，在c++中是name.pb.h，使用的时候直接按类来用就可以。

planning模块的很多逻辑走向是由配置文件决定的（通过将这部分内容从代码中剥离，可以方便的直接对配置文件进行调整，而不用编译源代码），conf文件夹统一存放配置文件，文件名后缀为pb.txt，和proto中的proto文件相对应，可以直接被proto文件生成的数据结构读取。**也就是proto定义了配置的field，conf指定了field的值**

## gflag

**使用gflag库，进行参数设置。**可以运行时从命令行改变参数，如果在命令行没有给出该参数的值则使用定义的默认值。一些文件路径等值可以只用这个库



# planning模块架构

[参考1](https://blog.csdn.net/davidhopper/article/details/89360385)

