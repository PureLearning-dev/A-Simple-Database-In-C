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


```c
#include <string.h>
char *strtok(char *str, const char *delim);
```

参数：

str — 要切割的字符串（第一次调用传入），后续切割传 NULL

delim — 分隔符字符串（可以是一个 / 多个字符，如空格、逗号、点）

返回值：返回本次切割出的字符串指针（token）；切割完毕返回 NULL



