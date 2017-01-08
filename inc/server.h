#ifndef SERVER_H
#define SERVER_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<mysql/mysql.h>
#include<pthread.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/epoll.h>
#include"define.h"
#include"threadpool.h"
#include"admin_operation.h"
#include"client_operation.h"

#define LISTENQ 20 
#define MAXLINE 1024

Pool * pool = NULL;
MYSQL* pdb;
pthread_mutex_t order_ticket_lock;
#endif
