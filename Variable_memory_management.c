#include <stdio.h>
#include <stdlib.h>
//#include <stdarg.h>

/* 循环首次适应法，所需功能如下
    1.分配
    2.释放：4种情况
    3.打印空闲分区链表
    4.主函数：读取命令行输入，分配 or 释放命令
*/

/* 空闲分区链表节点 */
struct free_m
{
    unsigned size_m;             //空闲区大小
    char *addr_m;                //空闲区首地址指针,单纯addr_m为地址
    struct free_m *next, *prior; //空闲区前后向指针
};

/* 全局变量 */
char *virtual_m_addr; //模拟内存起始地址
#define VIRTUAL_M_SIZE 1000 //申请的模拟内存字节数


struct free_m *now_m = NULL;      //当前查找指针
struct free_m *now_m_init = NULL; //起始查找指针，每次开始时都等于上一次分配查到的那块后面(分配成功)
                                  //主函数里，首先初始化为链表头head！！！
struct free_m *lookup_free_m = NULL; //释放时查找指针

struct free_m *head = NULL; //双向链表头

struct free_m *print_now = NULL; //当前打印分区

struct free_m *low_free_m = NULL;    //释放区相邻的低地址空闲分区
struct free_m *high_free_m = NULL;   //释放区向量的高地址空闲分区


/* 从头指针head开始，打印当前空闲分区链表信息(addr,size) */
void print_free_m_now()
{
    printf("---------------------------------------------\n");
    printf("当前空闲分区链表如下：\n");

    //首节点
    print_now = head;
    if(head == NULL)
    {
        printf("当前无空闲分区!\n");
        printf("/////////////////////////////////////////////\n");
    }
    else
    {
        printf("(addr=%p,size=%d)\n", print_now->addr_m, print_now->size_m);

        print_now = print_now->next;
        while (print_now != head)
        {
            printf("<->(addr=%p,size=%d)\n", print_now->addr_m, print_now->size_m);
            print_now = print_now->next;
        }
        printf("/////////////////////////////////////////////\n");
    }
    /*
    printf("继续输入以下命令:\n\
        \"m size\"：分配size bytes空间;\n\
        \"f size addr\": 释放相对于申请的1KB模拟内存首地址addr的size bytes空间;\n\
        \"q\"/\"Q\": 释放申请的1KB模拟内存并退出.\n");
    */
}

/* 内存分配函数 */
char *lmalloc(unsigned size)
{
    now_m = now_m_init;
    //从now_m_init开始循环扫描链表
    if(now_m == NULL) printf("分配失败: size=%d！当前无空闲分区，请先释放...\n",size);
    else
    {
        do
        {
            //若当前查找分区<分配所需大小，当前扫描指针下移
            if (now_m->size_m < size)
            {
                now_m = now_m->next;
            }
            //若当前查找分区>=分配所需大小
            else
            {
                //若新分区大小>0，修改即可
                if (now_m->size_m - size > 0)
                {
                    now_m->addr_m += size;
                    now_m->size_m -= size;
                }
                //否则，若新分区大小=0
                else
                {
                    if(now_m == head) //若分配的是链表头，需更新head
                    {
                        if(now_m->next == now_m)//若为仅有的1空闲分区
                            head = NULL;
                        else 
                        {
                            head = head->next;
                            //无论是否链表头，连接原前后节点操作相同
                            now_m->prior->next = now_m->next;
                            now_m->next->prior = now_m->prior;
                        } 
                    }

                }

                //成功分配，从链表头head开始打印当前空闲分区情况
                printf("分配成功: addr=%p,size=%d！\n",now_m->addr_m-size,size);
                print_free_m_now();

                //更新起始查找指针位置
                if(head ==NULL ) now_m_init =head;
                else now_m_init = now_m->next;

                //结束本次分配
                return now_m->addr_m;
            }
        } while (now_m->next != now_m_init); //当前查找指针尚未循环到初始查找指针

        //循环1轮后无符合大小的空闲分区
        printf("分配失败，无合适大小的空闲分区: size=%d！\n",size);
        print_free_m_now();
    }
}

/* 内存释放函数 */
char *lfree(unsigned size, int addr)
{
    char *absolute_addr = virtual_m_addr + addr;

    if(head==NULL)//当前无空闲分区，必可释放，释放区为首节点
    {
        head = (struct free_m *)malloc(sizeof(struct free_m));
        head->addr_m = absolute_addr;
        head->size_m = size;
        head->next = head;
        head->prior=head;
        now_m_init = head;
    }
    else//当前有空闲分区
    {
        //从链表头head开始，查找addr相邻的2个空闲分区起始地址
        lookup_free_m = head;

        //先判断起始地址相邻的上下分区，再判断错误情况

        //释放区起始地址<首空闲分区起始地址：释放区无低地址相邻空闲分区
        //=%p,插入起始地址=%p\n",lookup_free_m->addr_m,absolute_addr);
        if (lookup_free_m->addr_m > absolute_addr)
        {
            low_free_m = NULL;
            high_free_m = lookup_free_m; //首空闲分区,=head，链表头
        }
        else
        {
            lookup_free_m = lookup_free_m->next;

            //释放区起始地址=首空闲分区起始地址：无论size多少都会报错
            if(lookup_free_m->addr_m == absolute_addr)
            {
                //释放区在首空闲分区内：报错
                if(size <= lookup_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //释放区覆盖整个首空闲分区：报错
                else
                {
                    printf("Error: Release area overleap a whole free area!\n");
                    return(0);
                }
            }

            //释放区起始地址>首空闲分区起始地址：找下一个分区
            while (lookup_free_m->addr_m < absolute_addr)
            {
                lookup_free_m = lookup_free_m->next;
                //若查完最后1空闲分区
                if (lookup_free_m == head)
                    break;
            };

            //跳出循环：找到第一个"释放区起始地址<=首空闲分区起始地址"的空闲分区 or 查到链表尾也未找到
            //最后1空闲分区起始地址也在addr前：释放区无高地址相邻空闲分区
            if (lookup_free_m == head) 
            {
                low_free_m = lookup_free_m->prior; //最后1空闲分区
                high_free_m = NULL;
            }
            //找到第一个"释放区起始地址<首空闲分区起始地址"的空闲分区
            else if(lookup_free_m->addr_m > absolute_addr)
            {
                high_free_m = lookup_free_m;
                low_free_m = lookup_free_m->prior;
            }
            //找到第一个"释放区起始地址=首空闲分区起始地址"的空闲分区：总报错
            else
            {
                //释放区在空闲分区内：报错
                if(size <= lookup_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //释放区覆盖整个空闲分区：报错
                else
                {
                    printf("Error: Release area overleap a whole free area!\n");
                    return(0);
                }
            }
        }

        /* 释放区的4种情况*/

        //释放区起始地址<所有空闲区起始地址
        if (low_free_m == NULL)
        {
            
            //释放区后部分覆盖high_free_m
            if(absolute_addr+size > high_free_m->addr_m)
            {
                //覆盖整块high_free_m：报错
                if(absolute_addr+size >= high_free_m->addr_m + high_free_m->size_m)
                {
                    printf("Error: Release area overleap a whole free area!\n");
                    return(0);
                }
                //覆盖部分high_free_m：警报，但仍释放
                else
                {
                    //再看
                    //printf("高地址=%p,插入起始地址=%p\n",high_free_m->addr_m,absolute_addr);
                    printf("Warn: Back of release area overleap part of free area!\n");
                    char str1[20];
                    char str2[20];
                    sprintf(str1, "%d", high_free_m->addr_m);
                    sprintf(str2, "%d", absolute_addr);
                    //printf("高地址=%s,插入起始地址=%s\n",str1,str2);
                    int add_size = atoi(str1)-atoi(str2);
                    high_free_m->size_m += add_size;
                    high_free_m->addr_m = absolute_addr;
                }
            }
            //后邻空闲区
            else if (absolute_addr + size == high_free_m->addr_m)
            {
                high_free_m->size_m += size;
                high_free_m->addr_m -= size;
            }
            //后空闲区不相邻，新建空闲区
            else if(absolute_addr + size < high_free_m->addr_m)
            {
                struct free_m *new_m = NULL;                            //新建空闲分区指针
                new_m = (struct free_m *)malloc(sizeof(struct free_m)); //创建存新空闲分区信息的结构体

                //存储新空闲分区信息
                new_m->addr_m = absolute_addr;
                new_m->size_m = size;

                //插入链表头
                new_m->prior = head->prior; //即最后一个
                head->prior->next = new_m;
                new_m->next = head; //即表头
                head->prior = new_m;

                //表头指针更新
                head = new_m;
            }
        }
        //释放区起始地址>所有空闲区起始地址
        else if (high_free_m == NULL)
        {
            //释放区前部分覆盖low_free_m
            if(absolute_addr < low_free_m->addr_m + low_free_m->size_m)
            {
                //释放区全部位于low_free_m：报错
                if(absolute_addr+size <= low_free_m->addr_m + low_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //释放区仅前部分覆盖low_free_m：警报，但仍释放
                else
                {
                    printf("Warn: Front of release area overleap part of free area!\n");
                    char str1[20];
                    char str2[20];
                    sprintf(str1, "%d", low_free_m->addr_m);
                    sprintf(str2, "%d", absolute_addr);
                    //printf("高地址=%s,插入起始地址=%s\n",str1,str2);
                    int add_size = size - ( atoi(str1) - atoi(str2) );
                    low_free_m->size_m += add_size;
                }
            }
            //前邻空闲区
            else if (low_free_m->addr_m + low_free_m->size_m == absolute_addr)
            {
                low_free_m->size_m += size;
            }
            //前空闲区不相邻，插入链表尾
            else
            {
                struct free_m *new_m = NULL;                            //新建空闲分区指针
                new_m = (struct free_m *)malloc(sizeof(struct free_m)); //创建存新空闲分区信息的结构体

                //存储新空闲分区信息
                new_m->addr_m = absolute_addr;
                new_m->size_m = size;

                //插入链表最后
                new_m->prior = low_free_m; //即最后一个
                low_free_m->next = new_m;
                new_m->next = head; //即表头
                head->prior = new_m;
            }
        }
        //释放区起始地址 in 两空闲区起始地址之间
        else
        {
            //释放区起始地址 in low_free_m
            if(absolute_addr <= low_free_m->addr_m + low_free_m->size_m)
            {
                //释放区全部位于low_free_m：报错
                if(absolute_addr+size <= low_free_m->addr_m + low_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //释放区仅前部分覆盖low_free_m
                else
                {
                    //后半地址<high_free_m：警报，但仍释放，合并到low_free_m
                    if(absolute_addr+size < high_free_m->addr_m)
                    {
                        if(absolute_addr < low_free_m->addr_m + low_free_m->size_m) printf("Warn: Front of release area overleap part of free area!\n");
                        char str1[20];
                        char str2[20];
                        sprintf(str1, "%d", low_free_m->addr_m);
                        sprintf(str2, "%d", absolute_addr);
                        //printf("高地址=%s,插入起始地址=%s\n",str1,str2);
                        //int add_size = size - (atoi(str1) - atoi(str2));
                        low_free_m->size_m = atoi(str2)+size-atoi(str1);
                    }
                    //后半地址>=high_free_m      
                    else
                    {
                        //不覆盖整个high_free_m：警报，但仍释放，三块合并到low_free_m
                        if(absolute_addr+size < high_free_m->addr_m + high_free_m->size_m)
                        {
                            char str1[20];
                            char str2[20];
                            sprintf(str1, "%d", high_free_m->addr_m);
                            sprintf(str2, "%d", low_free_m->addr_m);
                            //printf("高地址=%s,插入起始地址=%s\n",str1,str2);
                            low_free_m->size_m = atoi(str1) - atoi(str2) + high_free_m->size_m;
                            low_free_m->next = high_free_m->next;
                        }
                        //覆盖整个high_free_m：报错
                        else
                        {
                            printf("Error: Release area overleap a whole free area!\n");
                            return(0);
                        }
                    }
                }    
            }
            //释放区起始地址在合法位置(与low_free_m不相邻)
            else if(absolute_addr < high_free_m->addr_m)
            {
                //释放区后部分覆盖high_free_m
                if(absolute_addr+size > high_free_m->addr_m)
                {
                    //覆盖整块high_free_m：报错
                    if(absolute_addr+size >= high_free_m->addr_m + high_free_m->size_m)
                    {
                        printf("Error: Release area overleap a whole free area!\n");
                        return(0);
                    }
                    //覆盖部分high_free_m：警报，但仍释放
                    else
                    {
                        printf("Warn: Back of release area overleap part of free area!\n");
                        char str1[20];
                        char str2[20];
                        sprintf(str1, "%d", high_free_m->addr_m);
                        sprintf(str2, "%d", absolute_addr);
                        int add_size = atoi(str1)-atoi(str2);
                        high_free_m->size_m += add_size;
                        high_free_m->addr_m = absolute_addr;
                    }
                }
                //后邻空闲区
                else if (absolute_addr + size == high_free_m->addr_m)
                {
                    high_free_m->size_m += size;
                    high_free_m->addr_m -= size;
                }
                //后空闲区不相邻，新建空闲区
                else if(absolute_addr + size < high_free_m->addr_m)
                {
                    struct free_m *new_m = NULL;                            //新建空闲分区指针
                    new_m = (struct free_m *)malloc(sizeof(struct free_m)); //创建存新空闲分区信息的结构体

                    //存储新空闲分区信息
                    new_m->addr_m = absolute_addr;
                    new_m->size_m = size;

                    /*插入low_free_m/high_free_m之间 */
                    new_m->prior = low_free_m;
                    low_free_m->next = new_m;

                    new_m->next = high_free_m;
                    high_free_m->prior = new_m;
                }
            }
        }
    }


    //成功释放，从链表头head开始打印当前空闲分区情况
    printf("释放成功: addr=%p,size=%d！\n",absolute_addr,size);
    print_free_m_now();
}

/* 主程序
    申请"模拟内存"=1KB
    创建空闲分区链表，链表头指针head
    以命令"m size"、"f size addr"实现在1KB模拟内存中分配内存、释放内存
*/
int main()
{
    /* malloc申请空间 */
    //////////////////////////////////////////////////

    //申请1KB内存
    virtual_m_addr = malloc(VIRTUAL_M_SIZE);

    //分配失败，返回NULL
    if (virtual_m_addr == NULL)
    {
        printf("Warn: There is no enough memory to create a virtual memory! Press enter to continue...\n");
        exit(1);
    }

    /* 初始化系统及空闲存储区 */
    ///////////////////////////////////////////////////////////////////

    //分配成功，返回首地址virtual_m_addr，大小为VIRTUAL_M_SIZE
    //创建双向链表
    //全局head：指向结构体(节点)的指针；=右侧：创建该指针指向的内存区(1个结构体)，=完成指向
    head = (struct free_m *)malloc(sizeof(struct free_m)); //不可无
    head->addr_m = virtual_m_addr;
    head->size_m = VIRTUAL_M_SIZE;
    head->prior = head;
    head->next = head;

    //初始化起始查找指针
    now_m_init = head;

    /* 输入指令 */
    ///////////////////////////////////////////////////////////////////
    int size;
    int addr;
    printf("申请的1KB模拟内存为：\n(addr=%p,size=%d)\n", virtual_m_addr, VIRTUAL_M_SIZE);
    printf("请输入以下命令:\n\
        \"m size\"：分配size bytes空间;\n\
        \"f size addr\": 释放相对于申请的1KB模拟内存首地址addr的size bytes空间;\n\
        \"p\": 打印当前空闲分区链表信息;\n\
        \"q\"/\"Q\": 释放申请的1KB模拟内存并退出.\n");

    char cmd_char;
    cmd_char = '\0';
    while (cmd_char != 'q')
    {
        //过滤缓冲区空白字符'\n'、'\t'、' '
        do
        {
            cmd_char = getchar();
        } while (cmd_char == '\n' || cmd_char == '\t' || cmd_char == ' ' || cmd_char == '\0');

        //首个非空白字符:m/f/其他
        switch (cmd_char)
        {
        case 'm': //分配内存
            scanf("%d", &size);
            //size>1000在lmalloc中处理
            if (size < 0)
            {
                printf("cmd_char=%c\n", cmd_char);
                printf("Error: Size must be unsigned! Press enter to continue...\n");
                break;
            }
            lmalloc(size);
            break; //否则会运行case 'f'
        case 'f':
            scanf("%d %d", &size, &addr);
            //参数输入错误
            //printf("%d,%d\n", size, addr);
            if (size < 0)
            {
                printf("Error: Size must be unsigned! Press enter to continue...\n");
                break;
            }
            else if (addr < 0 || addr > VIRTUAL_M_SIZE)
            {
                printf("Error: Relative addr must be 0 ~ 1000! Press enter to continue...\n");
                break;
            }
            //释放内存
            lfree(size, addr);
            break;
        case 'p':
            print_free_m_now();
            break;
        default:
        {
            printf("Error: Wrong command! Press enter to continue...\n");
            break;
        }
        }
    }

    //input='q'
    free(virtual_m_addr);
    return 0;
}