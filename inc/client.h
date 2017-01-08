#ifndef CLIENT_H
#define CLIENT_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "define.h"
#include "common.h"

int client_operate(int);
int query_ticket(int);
int query_all_cinema(int);
int query_a_cinema(int);
int query_a_movie(int);
int order_ticket(int);

void *handler(void *);

int login_success_flag = 0;

#endif
