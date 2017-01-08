#ifndef CLIENT_OPERATION_H
#define CLIENT_OPERATION_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mysql/mysql.h>
#include<pthread.h>
#include<arpa/inet.h>
#include"define.h"
#include"common.h"

void *ClientRegister(void *);
void *ClientLogin(void *);
void *QueryAllCinemaMovie(void *);
void *ClientQueryACinemaMovie(void *);
void *ClientQueryAMovie(void *);
void *ClientOrderTicket(void *);
extern MYSQL *pdb;
extern pthread_mutex_t order_ticket_lock;
#endif
