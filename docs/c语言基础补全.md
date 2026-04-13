对于宏，分为两种类型，一是直接替换，二是存在参数，将实参替代形参。

```c
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
```

因为直接替换的较为简单，不多赘述。上述给出的实例是带有形参的宏，使用这个宏时，会将下面使用时带有的参数替换进去。

`uint32_t`是一种精细化的32位无符号整数型数据，无关平台，常用于数据库之类的需要精细化计算内存地址的场景。

在创建自己的头文件后，为了让不管多少次引用都只进入一次，可以带上`#ifndef` + `#define` + `#endif`。

`main`函数可以有两个参数`int main(int argc, char *argv[])`，这个函数中的参数分别对应以下内容：

main 函数的两个参数：其中argv[0]是文件的名称，是固定的！

argc（argument count）：命令行参数的总数量，包含程序名本身，所以最小值是 1

argv（argument vector）：字符串数组，存储每个参数的具体内容