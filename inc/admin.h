#ifndef ADMIN_H
#define ADMIN_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"define.h"
#include"common.h"

int admin_operate(int);
int operate_cinema(int);
int add_cinema(int);
int query_cinema(int);
int delete_cinema(int);
int update_cinema(int);
void *handler(void *arg);
int operate_movie(int);
int add_movie(int);
int query_cinema_movie(int);
int update_cinema_movie(int);
int delete_cinema_movie(int);


int login_success_flag = 0;

#endif
