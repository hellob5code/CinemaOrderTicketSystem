#ifndef STRUCT_H
#define STRUCT_H

typedef			//消息头结构体
struct MSGHEAD
{
	int msglen;
	int msgtype;
}
MsgHead;

typedef			//注册信息结构体
struct REGISTERINFO
{
	char username[20];	//18
	char userpasswd[12];
	char phonenumber[12];
	char birthdate[12];
}
RegInfo;

typedef			//登陆信息结构体
struct LOGININFO
{
	char username[20];	//18
	char userpasswd[12];
}
Login;

typedef			//工作链表结点
struct WORK
{
	void *(*process)(void *arg);	//工作的回调函数（即该份工作要做到的函数）
	void *arg;
	struct WORK* next;
}
Work;

typedef			//线程池结构体
struct POOL
{
	pthread_mutex_t queue_lock;	//工作链表互斥锁
	pthread_cond_t queue_ready;	//状态变量

	Work * queue_head;				//工作链表头指针

	int shutdown;				//线程池标志 0：运行中 1：退出
	pthread_t *threadid;		//线程id数组

	int max_thread_num;			//线程池中线程数
	int cur_queue_size;			//工作任务链表长度
}
Pool;

typedef			//电影院信息结构体
struct CINEMAINFO
{
	char name[20];	//18
	char address[24];
	char telphone[12];
}
Cin;

typedef
struct MOVIEINFO
{
	char cinema[20];
	char name[24];
	char date[12];
	char time[12];
	int price;
	int seatleft;
}
Mov;
#define MAXLINE 1024
#endif
