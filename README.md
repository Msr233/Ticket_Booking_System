# 1、程序使用方法

在 `./Ticket_Booking_System`文件夹下运行

```bash
./main <path>
```

其中path为可选项，用于指定读取的数据文件。不输入默认读取 `./txt/show.txt` 数据。

# 2、配置说明

## 编译

`./Ticket_Booking_System` 中已包含了编译完成的可执行文件，如果需要可重新编译。

在 `./Ticket_Booking_System`文件夹下执行

```bash
make clean
make
```

## 参数修改

默认情况下用于记录客户操作的字典最多可记录 64 名客户，每个客户姓名长度最多为16字节，所有操作字符串长度之和最大为1024字节。

+ 最大客户数量：64
+ 客户姓名：16 Byte
+ 操作记录上限：1024 Byte

相关参数定义在 `./include/dictionary_shm.h` 中

```C
#define VALUE_MAX_SIZE 1024	// 操作记录上限
#define NAME_MAX 16			// 客户姓名长度
#define CLIENT_NUM_MAX 64	// 最大客户数量
```

# 3、项目结构

```C
|-- Ticket_Booking_System
|    |-- include
|    |    |-- 相关代码
|    |-- test
|    |    |--测试代码
|    |-- txt
|    |    |-- 测试样例
|    |-- unused
|    |    |-- 未使用的代码
|    |-- main.c
|    |-- Makefile
```

# 4、输入数据格式描述

```txt
n // 音乐厅座位排数
m // 每排座位数
k // 代理个数
rvt // 预约的有效期（单位分钟）
agent 1
reserve reserve_time (单位秒) 
ticket ticket_time (单位秒) 
cancel cancel_time (单位秒) 
check_customer check_time (单位秒) 
 : 
有效的交易列表 
 :
end 
 : 
 : 
 : 
agent k 
reserve reserve_time (单位秒) 
ticket ticket_time (单位秒) 
cancel cancel_time (单位秒) 
check_customer check_time (单位秒) 
 : 
有效的交易列表 
 : 
end

有效的交易如下：
reserve <座位描述> name_of_customer // 为某客户预订指定座位的票 
ticket <座位描述> name_of_customer // 为某客户购买指定座位的票 
cancel <座位描述> name_of_customer // 为某客户取消指定座位的预订 
show all name_of_customer // 显示某客户的预订或购票情况

<座位描述>可以为：
A 2 // 第 A 行，第 2 座 A 4 7 // 第 A 行，第 4 到 7 座 A // 第 A 行
A 2r // 从第 A 行开始的 2 行
A 2c // 第 A 行，2 个座位（不在意具体列）
6 // 6 个座位（不在意具体行和列）

```
