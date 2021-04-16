# OperatingSystem_2021

homework for course OperatingSystem_2021

# 一、可变分区存储管理

## 题目

编写一个C语言程序，模拟UNIX的可变分区内存管理，使用循环首次适应法实现对一块内存区域的分配和释放管理。

## 分析

节点：空闲分区size、空闲分区起始地址、前分区、后分区

1. 申请一块空间1KB空间，模拟整个“主存”

   ```c
   free_m  * main_m = (free_m  * ) malloc(1000);
   ```

2. 以 main_为起点，创建空闲分区双向链表

   起初，链表头节点取main_m

   next/prior 为自身

   seze  = 1000

3. 起初：链表仅1节点，main_m指向该节点，为整个内存，节点取值size=1000 (B，单位字节，同malloc函数)

4. 2函数：

   

### lmalloc(unsigned size)

从头(指针head)循环扫描空闲分区链表——设置1个当前位置指针

```c
free_m  * now = NULL;
```

对比size/循环到的**每个节点**->size

```c
free_m * now_m_init = now;//起始查找位置
do
{
    if(size < now->size)
    now = now->next
else if( >= )
{
    //正好
    if( == )
    {
        //now前后的节点连接
    }
    //一分为二
    else
    {
        //分配的空间：addr=now->addr，size=参数size
    //余下：addr=now->addr + size ,size = now->size - 参数size
    free_m *new_m =NULL;
	new_m->addr = ;
	new_m->size = ;
    //new_m插入：从头扫描
    //若size小于自己，取下一m；若大于自己，停，
    //和大于自己的m连接；和大于自己的m的prior连接
    }
    //输出当前循环链表：从链表头指针head开始循环
    printf("分配成功。当前空闲分区情况为(addr,size)：");
    //print_m
    free_m * print_m = head;
    printf("(%s,%d)",print_m->addr,print_m->size);
    while(free_m != head)
    {
        print_m = print_m->next;
        printf(" <-> (%s,%d)",print_m->addr,print_m->size);
    }
    
}
while(now != now_m_init)//回到now_m_init
	
    
```

### 释放函数

判断释放区域的上下相邻区域是否空闲分区

释放区域：addr，size



遍历addr上下的空闲分区addr

上：判断addr 是否等于 某空闲分区addr+size？是=1

low_free_m 指针



下：判断addr+size 是否等于某空闲分区的addr？是=1

high_free_m 指针



4情况：

1 0：low_free_m的addr不变，size增加size

0 1：high_free_m的addr-size，size增加size

1 1：low_free_m的addr不变，
size增加size+high_free_m->size
prior不变
next=high_free_m->next

0 0：新增 对new_m内部值赋值
addr=addr，size=size
head开始对比addr值，查到前一个<，后一个>
插入在这中间