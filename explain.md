## 秦 OS

#### questions
- [x] 如果调用门的16位段选择子是LDT段选择子会发生什么？

    答：会报错“call_protected: selected descriptor is not code”。应该是系统不允许有这种格式

- [x] 如果调用门的16位段选择子的RPL不是0段会发生什么？

    答：目前没看出有什么影响

- [ ] 程序编译、加载执行过程中，操作系统是如何确定程序的RPL的？
- [ ] ds、fs、es 和 gs 在 ret 指令返回时，会被检查指向的段的 DPL 是否小于 CPL，是则清0。那么利用 call gate 向更高特权级转移后然后返回时，这些寄存器都会应该需要保存吧？
- [ ] lldt 指令能够在非0特权级执行吗？如果能的话，那么一个进程就可以访问其他进程的代码了。
- [ ] ARDS [address range descriptor structure] 是和bios相关的一个东西，这个是什么？

#### tips
保护模式下，汇编在执行 `mov es, ax` 这种将数据加载到段寄存器的命令也会检查 ax 中选择符的特权级。

`ret` 和 `retf` 的区别：两者分别对应 call 指令的短跳转和长跳转。`call short` 执行时会向栈中压入 `eip`，`call long` 则向栈中压入 `cs`、`eip`。而 `ret` 是从短跳转中返回，从栈中弹出 `eip`，而 `retf` 从长跳转中返回，从栈中弹出 `cs`、`eip`。