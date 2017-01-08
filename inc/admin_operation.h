#ifndef ADMIN_OPERATION_H
#define ADMIN_OPERATION_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mysql/mysql.h>
#include<arpa/inet.h>
#include "define.h"
#include "common.h"
//#include "admin_operation.h"

//int send_msg(int,int,int,void*,int);
void *AdminRegister(void *);
void *sLogin(void *);
void *AddCinema(void *);
void *QueryAllCinema(void *);
void *DeleteACinema(void *);
void *UpdateACinema(void *);

void *AddAMovie(void *);
void *AdminQueryACinema(void *);
void *DeleteAMovie(void *);
void *UpdateAMovie(void *);
extern MYSQL *pdb;

#endif
