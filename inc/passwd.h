#ifndef PASSWD_H
#define PASSWD_H

#include<stdio.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>
#include<assert.h>
#include<string.h>

int getch(void);
int getpasswd(char *passwd,int num);

#endif
