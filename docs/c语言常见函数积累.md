```c
#include <stdio.h>
int sscanf(const char *str, const char *format, ...);
```
参数：

str — 要解析的源字符串

format — 格式字符串（和 scanf 完全一样的格式符）

... — 接收解析结果的变量指针

返回值： 成功解析的变量个数，失败返回 EOF


```c
#include <string.h>
void *memcpy(void *dst, const void *src, size_t n);
```

参数:

dst — 目标地址，数据复制到哪里

src — 源地址，数据从哪里来

n — 复制多少字节

返回值：返回 dst 指针本身（一般不用）

