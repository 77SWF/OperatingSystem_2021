#include <stdio.h>
#include <stdlib.h>
//#include <stdarg.h>

/* ѭ���״���Ӧ�������蹦������
    1.����
    2.�ͷţ�4�����
    3.��ӡ���з�������
    4.����������ȡ���������룬���� or �ͷ�����
*/

/* ���з�������ڵ� */
struct free_m
{
    unsigned size_m;             //��������С
    char *addr_m;                //�������׵�ַָ��,����addr_mΪ��ַ
    struct free_m *next, *prior; //������ǰ����ָ��
};

/* ȫ�ֱ��� */
char *virtual_m_addr; //ģ���ڴ���ʼ��ַ
#define VIRTUAL_M_SIZE 1000 //�����ģ���ڴ��ֽ���


struct free_m *now_m = NULL;      //��ǰ����ָ��
struct free_m *now_m_init = NULL; //��ʼ����ָ�룬ÿ�ο�ʼʱ��������һ�η���鵽���ǿ����(����ɹ�)
                                  //����������ȳ�ʼ��Ϊ����ͷhead������
struct free_m *lookup_free_m = NULL; //�ͷ�ʱ����ָ��

struct free_m *head = NULL; //˫������ͷ

struct free_m *print_now = NULL; //��ǰ��ӡ����

struct free_m *low_free_m = NULL;    //�ͷ������ڵĵ͵�ַ���з���
struct free_m *high_free_m = NULL;   //�ͷ��������ĸߵ�ַ���з���


/* ��ͷָ��head��ʼ����ӡ��ǰ���з���������Ϣ(addr,size) */
void print_free_m_now()
{
    printf("---------------------------------------------\n");
    printf("��ǰ���з����������£�\n");

    //�׽ڵ�
    print_now = head;
    if(head == NULL)
    {
        printf("��ǰ�޿��з���!\n");
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
    printf("����������������:\n\
        \"m size\"������size bytes�ռ�;\n\
        \"f size addr\": �ͷ�����������1KBģ���ڴ��׵�ַaddr��size bytes�ռ�;\n\
        \"q\"/\"Q\": �ͷ������1KBģ���ڴ沢�˳�.\n");
    */
}

/* �ڴ���亯�� */
char *lmalloc(unsigned size)
{
    now_m = now_m_init;
    //��now_m_init��ʼѭ��ɨ������
    if(now_m == NULL) printf("����ʧ��: size=%d����ǰ�޿��з����������ͷ�...\n",size);
    else
    {
        do
        {
            //����ǰ���ҷ���<���������С����ǰɨ��ָ������
            if (now_m->size_m < size)
            {
                now_m = now_m->next;
            }
            //����ǰ���ҷ���>=���������С
            else
            {
                //���·�����С>0���޸ļ���
                if (now_m->size_m - size > 0)
                {
                    now_m->addr_m += size;
                    now_m->size_m -= size;
                }
                //�������·�����С=0
                else
                {
                    if(now_m == head) //�������������ͷ�������head
                    {
                        if(now_m->next == now_m)//��Ϊ���е�1���з���
                            head = NULL;
                        else 
                        {
                            head = head->next;
                            //�����Ƿ�����ͷ������ԭǰ��ڵ������ͬ
                            now_m->prior->next = now_m->next;
                            now_m->next->prior = now_m->prior;
                        } 
                    }

                }

                //�ɹ����䣬������ͷhead��ʼ��ӡ��ǰ���з������
                printf("����ɹ�: addr=%p,size=%d��\n",now_m->addr_m-size,size);
                print_free_m_now();

                //������ʼ����ָ��λ��
                if(head ==NULL ) now_m_init =head;
                else now_m_init = now_m->next;

                //�������η���
                return now_m->addr_m;
            }
        } while (now_m->next != now_m_init); //��ǰ����ָ����δѭ������ʼ����ָ��

        //ѭ��1�ֺ��޷��ϴ�С�Ŀ��з���
        printf("����ʧ�ܣ��޺��ʴ�С�Ŀ��з���: size=%d��\n",size);
        print_free_m_now();
    }
}

/* �ڴ��ͷź��� */
char *lfree(unsigned size, int addr)
{
    char *absolute_addr = virtual_m_addr + addr;

    if(head==NULL)//��ǰ�޿��з������ؿ��ͷţ��ͷ���Ϊ�׽ڵ�
    {
        head = (struct free_m *)malloc(sizeof(struct free_m));
        head->addr_m = absolute_addr;
        head->size_m = size;
        head->next = head;
        head->prior=head;
        now_m_init = head;
    }
    else//��ǰ�п��з���
    {
        //������ͷhead��ʼ������addr���ڵ�2�����з�����ʼ��ַ
        lookup_free_m = head;

        //���ж���ʼ��ַ���ڵ����·��������жϴ������

        //�ͷ�����ʼ��ַ<�׿��з�����ʼ��ַ���ͷ����޵͵�ַ���ڿ��з���
        //=%p,������ʼ��ַ=%p\n",lookup_free_m->addr_m,absolute_addr);
        if (lookup_free_m->addr_m > absolute_addr)
        {
            low_free_m = NULL;
            high_free_m = lookup_free_m; //�׿��з���,=head������ͷ
        }
        else
        {
            lookup_free_m = lookup_free_m->next;

            //�ͷ�����ʼ��ַ=�׿��з�����ʼ��ַ������size���ٶ��ᱨ��
            if(lookup_free_m->addr_m == absolute_addr)
            {
                //�ͷ������׿��з����ڣ�����
                if(size <= lookup_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //�ͷ������������׿��з���������
                else
                {
                    printf("Error: Release area overleap a whole free area!\n");
                    return(0);
                }
            }

            //�ͷ�����ʼ��ַ>�׿��з�����ʼ��ַ������һ������
            while (lookup_free_m->addr_m < absolute_addr)
            {
                lookup_free_m = lookup_free_m->next;
                //���������1���з���
                if (lookup_free_m == head)
                    break;
            };

            //����ѭ�����ҵ���һ��"�ͷ�����ʼ��ַ<=�׿��з�����ʼ��ַ"�Ŀ��з��� or �鵽����βҲδ�ҵ�
            //���1���з�����ʼ��ַҲ��addrǰ���ͷ����޸ߵ�ַ���ڿ��з���
            if (lookup_free_m == head) 
            {
                low_free_m = lookup_free_m->prior; //���1���з���
                high_free_m = NULL;
            }
            //�ҵ���һ��"�ͷ�����ʼ��ַ<�׿��з�����ʼ��ַ"�Ŀ��з���
            else if(lookup_free_m->addr_m > absolute_addr)
            {
                high_free_m = lookup_free_m;
                low_free_m = lookup_free_m->prior;
            }
            //�ҵ���һ��"�ͷ�����ʼ��ַ=�׿��з�����ʼ��ַ"�Ŀ��з������ܱ���
            else
            {
                //�ͷ����ڿ��з����ڣ�����
                if(size <= lookup_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //�ͷ��������������з���������
                else
                {
                    printf("Error: Release area overleap a whole free area!\n");
                    return(0);
                }
            }
        }

        /* �ͷ�����4�����*/

        //�ͷ�����ʼ��ַ<���п�������ʼ��ַ
        if (low_free_m == NULL)
        {
            
            //�ͷ����󲿷ָ���high_free_m
            if(absolute_addr+size > high_free_m->addr_m)
            {
                //��������high_free_m������
                if(absolute_addr+size >= high_free_m->addr_m + high_free_m->size_m)
                {
                    printf("Error: Release area overleap a whole free area!\n");
                    return(0);
                }
                //���ǲ���high_free_m�������������ͷ�
                else
                {
                    //�ٿ�
                    //printf("�ߵ�ַ=%p,������ʼ��ַ=%p\n",high_free_m->addr_m,absolute_addr);
                    printf("Warn: Back of release area overleap part of free area!\n");
                    char str1[20];
                    char str2[20];
                    sprintf(str1, "%d", high_free_m->addr_m);
                    sprintf(str2, "%d", absolute_addr);
                    //printf("�ߵ�ַ=%s,������ʼ��ַ=%s\n",str1,str2);
                    int add_size = atoi(str1)-atoi(str2);
                    high_free_m->size_m += add_size;
                    high_free_m->addr_m = absolute_addr;
                }
            }
            //���ڿ�����
            else if (absolute_addr + size == high_free_m->addr_m)
            {
                high_free_m->size_m += size;
                high_free_m->addr_m -= size;
            }
            //������������ڣ��½�������
            else if(absolute_addr + size < high_free_m->addr_m)
            {
                struct free_m *new_m = NULL;                            //�½����з���ָ��
                new_m = (struct free_m *)malloc(sizeof(struct free_m)); //�������¿��з�����Ϣ�Ľṹ��

                //�洢�¿��з�����Ϣ
                new_m->addr_m = absolute_addr;
                new_m->size_m = size;

                //��������ͷ
                new_m->prior = head->prior; //�����һ��
                head->prior->next = new_m;
                new_m->next = head; //����ͷ
                head->prior = new_m;

                //��ͷָ�����
                head = new_m;
            }
        }
        //�ͷ�����ʼ��ַ>���п�������ʼ��ַ
        else if (high_free_m == NULL)
        {
            //�ͷ���ǰ���ָ���low_free_m
            if(absolute_addr < low_free_m->addr_m + low_free_m->size_m)
            {
                //�ͷ���ȫ��λ��low_free_m������
                if(absolute_addr+size <= low_free_m->addr_m + low_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //�ͷ�����ǰ���ָ���low_free_m�������������ͷ�
                else
                {
                    printf("Warn: Front of release area overleap part of free area!\n");
                    char str1[20];
                    char str2[20];
                    sprintf(str1, "%d", low_free_m->addr_m);
                    sprintf(str2, "%d", absolute_addr);
                    //printf("�ߵ�ַ=%s,������ʼ��ַ=%s\n",str1,str2);
                    int add_size = size - ( atoi(str1) - atoi(str2) );
                    low_free_m->size_m += add_size;
                }
            }
            //ǰ�ڿ�����
            else if (low_free_m->addr_m + low_free_m->size_m == absolute_addr)
            {
                low_free_m->size_m += size;
            }
            //ǰ�����������ڣ���������β
            else
            {
                struct free_m *new_m = NULL;                            //�½����з���ָ��
                new_m = (struct free_m *)malloc(sizeof(struct free_m)); //�������¿��з�����Ϣ�Ľṹ��

                //�洢�¿��з�����Ϣ
                new_m->addr_m = absolute_addr;
                new_m->size_m = size;

                //�����������
                new_m->prior = low_free_m; //�����һ��
                low_free_m->next = new_m;
                new_m->next = head; //����ͷ
                head->prior = new_m;
            }
        }
        //�ͷ�����ʼ��ַ in ����������ʼ��ַ֮��
        else
        {
            //�ͷ�����ʼ��ַ in low_free_m
            if(absolute_addr <= low_free_m->addr_m + low_free_m->size_m)
            {
                //�ͷ���ȫ��λ��low_free_m������
                if(absolute_addr+size <= low_free_m->addr_m + low_free_m->size_m)
                {
                    printf("Error: Already released!\n");
                    return(0);
                }
                //�ͷ�����ǰ���ָ���low_free_m
                else
                {
                    //����ַ<high_free_m�������������ͷţ��ϲ���low_free_m
                    if(absolute_addr+size < high_free_m->addr_m)
                    {
                        if(absolute_addr < low_free_m->addr_m + low_free_m->size_m) printf("Warn: Front of release area overleap part of free area!\n");
                        char str1[20];
                        char str2[20];
                        sprintf(str1, "%d", low_free_m->addr_m);
                        sprintf(str2, "%d", absolute_addr);
                        //printf("�ߵ�ַ=%s,������ʼ��ַ=%s\n",str1,str2);
                        //int add_size = size - (atoi(str1) - atoi(str2));
                        low_free_m->size_m = atoi(str2)+size-atoi(str1);
                    }
                    //����ַ>=high_free_m      
                    else
                    {
                        //����������high_free_m�������������ͷţ�����ϲ���low_free_m
                        if(absolute_addr+size < high_free_m->addr_m + high_free_m->size_m)
                        {
                            char str1[20];
                            char str2[20];
                            sprintf(str1, "%d", high_free_m->addr_m);
                            sprintf(str2, "%d", low_free_m->addr_m);
                            //printf("�ߵ�ַ=%s,������ʼ��ַ=%s\n",str1,str2);
                            low_free_m->size_m = atoi(str1) - atoi(str2) + high_free_m->size_m;
                            low_free_m->next = high_free_m->next;
                        }
                        //��������high_free_m������
                        else
                        {
                            printf("Error: Release area overleap a whole free area!\n");
                            return(0);
                        }
                    }
                }    
            }
            //�ͷ�����ʼ��ַ�ںϷ�λ��(��low_free_m������)
            else if(absolute_addr < high_free_m->addr_m)
            {
                //�ͷ����󲿷ָ���high_free_m
                if(absolute_addr+size > high_free_m->addr_m)
                {
                    //��������high_free_m������
                    if(absolute_addr+size >= high_free_m->addr_m + high_free_m->size_m)
                    {
                        printf("Error: Release area overleap a whole free area!\n");
                        return(0);
                    }
                    //���ǲ���high_free_m�������������ͷ�
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
                //���ڿ�����
                else if (absolute_addr + size == high_free_m->addr_m)
                {
                    high_free_m->size_m += size;
                    high_free_m->addr_m -= size;
                }
                //������������ڣ��½�������
                else if(absolute_addr + size < high_free_m->addr_m)
                {
                    struct free_m *new_m = NULL;                            //�½����з���ָ��
                    new_m = (struct free_m *)malloc(sizeof(struct free_m)); //�������¿��з�����Ϣ�Ľṹ��

                    //�洢�¿��з�����Ϣ
                    new_m->addr_m = absolute_addr;
                    new_m->size_m = size;

                    /*����low_free_m/high_free_m֮�� */
                    new_m->prior = low_free_m;
                    low_free_m->next = new_m;

                    new_m->next = high_free_m;
                    high_free_m->prior = new_m;
                }
            }
        }
    }


    //�ɹ��ͷţ�������ͷhead��ʼ��ӡ��ǰ���з������
    printf("�ͷųɹ�: addr=%p,size=%d��\n",absolute_addr,size);
    print_free_m_now();
}

/* ������
    ����"ģ���ڴ�"=1KB
    �������з�����������ͷָ��head
    ������"m size"��"f size addr"ʵ����1KBģ���ڴ��з����ڴ桢�ͷ��ڴ�
*/
int main()
{
    /* malloc����ռ� */
    //////////////////////////////////////////////////

    //����1KB�ڴ�
    virtual_m_addr = malloc(VIRTUAL_M_SIZE);

    //����ʧ�ܣ�����NULL
    if (virtual_m_addr == NULL)
    {
        printf("Warn: There is no enough memory to create a virtual memory! Press enter to continue...\n");
        exit(1);
    }

    /* ��ʼ��ϵͳ�����д洢�� */
    ///////////////////////////////////////////////////////////////////

    //����ɹ��������׵�ַvirtual_m_addr����СΪVIRTUAL_M_SIZE
    //����˫������
    //ȫ��head��ָ��ṹ��(�ڵ�)��ָ�룻=�Ҳࣺ������ָ��ָ����ڴ���(1���ṹ��)��=���ָ��
    head = (struct free_m *)malloc(sizeof(struct free_m)); //������
    head->addr_m = virtual_m_addr;
    head->size_m = VIRTUAL_M_SIZE;
    head->prior = head;
    head->next = head;

    //��ʼ����ʼ����ָ��
    now_m_init = head;

    /* ����ָ�� */
    ///////////////////////////////////////////////////////////////////
    int size;
    int addr;
    printf("�����1KBģ���ڴ�Ϊ��\n(addr=%p,size=%d)\n", virtual_m_addr, VIRTUAL_M_SIZE);
    printf("��������������:\n\
        \"m size\"������size bytes�ռ�;\n\
        \"f size addr\": �ͷ�����������1KBģ���ڴ��׵�ַaddr��size bytes�ռ�;\n\
        \"p\": ��ӡ��ǰ���з���������Ϣ;\n\
        \"q\"/\"Q\": �ͷ������1KBģ���ڴ沢�˳�.\n");

    char cmd_char;
    cmd_char = '\0';
    while (cmd_char != 'q')
    {
        //���˻������հ��ַ�'\n'��'\t'��' '
        do
        {
            cmd_char = getchar();
        } while (cmd_char == '\n' || cmd_char == '\t' || cmd_char == ' ' || cmd_char == '\0');

        //�׸��ǿհ��ַ�:m/f/����
        switch (cmd_char)
        {
        case 'm': //�����ڴ�
            scanf("%d", &size);
            //size>1000��lmalloc�д���
            if (size < 0)
            {
                printf("cmd_char=%c\n", cmd_char);
                printf("Error: Size must be unsigned! Press enter to continue...\n");
                break;
            }
            lmalloc(size);
            break; //���������case 'f'
        case 'f':
            scanf("%d %d", &size, &addr);
            //�����������
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
            //�ͷ��ڴ�
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