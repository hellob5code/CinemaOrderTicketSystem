#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<pthread.h>
#include<errno.h>
#include"define.h"

void pool_init(Pool**,int max_thread_num);	//线程池初始化
int pool_add_work(Pool**,void* (*process)(void *arg),void *arg);	//添加工作任务
int pool_destroy(Pool**);			//线程池销毁
void *thread_routine(void *arg);	//线程函数

extern Pool *pool;

#endif
