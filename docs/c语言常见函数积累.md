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


```c
#include <fcntl.h>
int open(const char *path, int flags, mode_t mode);
```

参数：

path — 文件路径

flags — 打开方式，如 O_RDWR | O_CREAT

mode — 创建文件时的权限，如 S_IWUSR | S_IRUSR

返回值： 成功返回文件描述符（非负整数），失败返回 -1


```c
#include <unistd.h>
int close(int fd);
```

参数：

fd — 要关闭的文件描述符

返回值： 成功返回 0，失败返回 -1

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```

参数：

fd — 要读取的文件描述符

buf — 读取的数据存到哪块内存

count — 最多读取多少字节


```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```

参数：

fd — 要写入哪个文件（文件描述符）

buf — 要写入的数据来源（内存地址）

count — 要写入多少字节

返回值：成功返回实际写入的字节数（ssize_t 是带符号的size_t），失败返回 -1


```c
#include <unistd.h>
lseek(fd, offset, whence)
```

参数：

fd — 要操作哪个文件（文件描述符）

offset — 移动多少字节

whence — 从哪里开始算偏移

返回值：成功返回移动后的文件指针位置（字节数），失败返回 -1。

whence 的三种取值：
cSEEK_SET    // 从文件开头算，最常用
SEEK_CUR    // 从当前位置算
SEEK_END    // 从文件末尾算（可以是负数往回移）

