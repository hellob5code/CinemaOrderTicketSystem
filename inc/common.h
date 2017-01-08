#ifndef COMMON_H
#define COMMON_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<mysql/mysql.h>
#include"define.h"
#include"passwd.h"

int Register(int,int);
int LogIn(int,int);
int send_msg(int,int,int,void *,int);
int QueryACinemaMovie(MYSQL*,int,int,char*);

int send_to_server(int fd,int type,char *buff,int len);

char id[18];
#endif
