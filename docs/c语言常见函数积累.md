```c
#include <stdio.h>
int sscanf(const char *str, const char *format, ...);
```
参数：

str — 要解析的源字符串

format — 格式字符串（和 scanf 完全一样的格式符）

... — 接收解析结果的变量指针

返回值： 成功解析的变量个数，失败返回 EOF

