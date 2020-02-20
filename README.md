## 秦 OS

### 粗略的代码整理

#### type.h
定义一些简单的 type 类型

#### sys.h
    定义一些系统自己使用的类型：
        Descriptor
        Gate

#### trap.c
    初始化中断控制器
    中断处理函数
    设置中断向量表
