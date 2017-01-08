#include "../inc/threadpool.h"

void pool_init(Pool **p,int max_thread_num)
{
	*p = (Pool*)malloc(sizeof(Pool));
	pthread_mutex_init(&((*p)->queue_lock),NULL);
	pthread_cond_init(&((*p)->queue_ready),NULL);

	(*p)->queue_head = NULL;
	(*p)->max_thread_num = max_thread_num;
	(*p)->cur_queue_size = 0;

	(*p)->shutdown = 0;
	(*p)->threadid = (pthread_t *)malloc(max_thread_num * sizeof(pthread_t));
	int i =0;
	int ret;
	int *thread_count = (int *)malloc(sizeof(int)*max_thread_num);
	for(i = 0;i< max_thread_num;i++)
	{
		thread_count[i] = i;
		ret = pthread_create(&((*p)->threadid[i]),NULL,thread_routine,thread_count+i);
	}
}

int pool_add_work(Pool** p,void *(*process)(void *arg),void *arg)
{
	Work * newwork = (Work *)malloc(sizeof(Work));
	newwork->process = process;
	newwork->arg = arg;
	newwork->next = NULL;

	pthread_mutex_lock(&((*p)->queue_lock));
	Work* member = pool->queue_head;
	if(member != NULL)
	{
		while(member->next != NULL)
		{
			member = member->next;
		}
		member->next = newwork;
	}
	else
	{
		(*p)->queue_head = newwork;
	}
	(*p)->cur_queue_size++;
	pthread_mutex_unlock(&((*p)->queue_lock));
	pthread_cond_signal(&((*p)->queue_ready));
	return 0;
}

int pool_destroy(Pool **p)
{
	if((*p)->shutdown)
	{
		return -1;
	}
	(*p)->shutdown = 1;

	pthread_cond_broadcast(&((*p)->queue_ready));

	int i;
	for(i = 0;i<(*p)->max_thread_num;i++)
	{
		pthread_join((*p)->threadid[i],NULL);
	}
	free((*p)->threadid);
	Work *head = NULL;
	while((*p)->queue_head != NULL)
	{
		head = (*p)->queue_head;
		(*p)->queue_head = (*p)->queue_head->next;
		free(head);
	}

	pthread_mutex_destroy(&((*p)->queue_lock));
	pthread_cond_destroy(&((*p)->queue_ready));
	free(*p);
	*p = NULL;
	return 0;
}

void * thread_routine(void *arg)
{
	printf("starting thread %d\n",*(int *)arg);
	while(1)
	{
		pthread_mutex_lock(&(pool->queue_lock));
		while(pool->cur_queue_size == 0 && !pool->shutdown)
		{
			printf("thread %d is waiting\n",*(int *)arg);
			pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
		}
		if(pool->shutdown)
		{
			pthread_mutex_unlock(&(pool->queue_lock));
			printf("thread %d will exit",*(int *)arg);
			pthread_exit(NULL);
		}
		printf("thread %d start to work\n",*(int *)arg);
		pool->cur_queue_size--;
		Work* work = pool->queue_head;
		pool->queue_head = work->next;
		pthread_mutex_unlock(&(pool->queue_lock));

		(*(work->process))(work->arg);
		free(work);
		work = NULL;
	}
}
