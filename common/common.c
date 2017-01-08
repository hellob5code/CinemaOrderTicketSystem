#include"../inc/common.h"

int send_to_server(int fd,int type,char *p,int len)
{
	MsgHead head;
	memset(&head,0,sizeof(MsgHead));
	head.msglen = sizeof(MsgHead) + len;
	head.msgtype = type;
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);

	char buff[MAXLINE] = {0};
	memcpy(buff,&head,sizeof(MsgHead));
	if(p != NULL)
		memcpy(buff+sizeof(MsgHead),p,len);
	int ret;
	ret = write(fd,buff,len + sizeof(MsgHead));
	if(ret <= 0)
		return -1;
	return 0;
}
int send_msg(int fd,int type,int flag,void *p,int len)
{
	int ret;
	MsgHead temp;
	char buff[MAXLINE] = {0};
	temp.msgtype = htonl(type);
	temp.msglen = htonl(sizeof(int)+sizeof(MsgHead)+len);
	flag = htonl(flag);
	memcpy(buff,&temp,sizeof(MsgHead));
	memcpy(buff+sizeof(MsgHead),&flag,sizeof(int));
//	while(len > )//如果len > MAXLINE-sizeof(MsgHead)-sizeof(int)，则不能发送完全（len > 1024-8-4）
//那时要超过18条电影院信息，sizeof(Cin) == 56
	if(p != NULL)
	{
		memcpy(buff+sizeof(MsgHead)+sizeof(int),p,len);
		free(p);
	}
	ret = write(fd,buff,ntohl(temp.msglen));
	if(ret < 0)
	{
		perror("write return");
		return -1;
	}
	return 0;
}

//注册专用函数，fd为服务器文件描述符，type为消息头类型，客户版为1,管理员版为101
int Register(int fd,int type)
{
	int ret;
	RegInfo temp;
	memset(&temp,0,sizeof(RegInfo));
	printf("请输入用户名  ：");
	scanf("%s",temp.username);
	getchar();
	printf("请输入密码    ：");
	getpasswd(temp.userpasswd,12);
	putchar('\n');
//	scanf("%s",temp.userpasswd);
	printf("请输入电话号码：");
	scanf("%s",temp.phonenumber);
	printf("请输入出生日期：");
	scanf("%s",temp.birthdate);
	
	char buff[MAXLINE] = {0};
	MsgHead head;
	head.msglen = sizeof(MsgHead) + sizeof(RegInfo);
	head.msgtype = type;
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);
	memcpy(buff,&head,sizeof(MsgHead));
	memcpy(buff+sizeof(MsgHead),&temp,sizeof(RegInfo));
	ret = write(fd,buff,ntohl(head.msglen));
	if(ret < 0)
	{
		printf("write fail\n");
		return -1;
	}
	else if(ret == 0)
	{
		printf("对端关闭");
		return 1;
	}
	return 0;
}
//登陆专用函数，fd为服务器文件描述符，type为消息头类型，客户版为3,管理员版为103
int LogIn(int fd,int type)
{
	int ret;
	RegInfo temp;
	memset(&temp,0,sizeof(RegInfo));
	printf("请输入用户名  ：");
	scanf("%s",temp.username);
	getchar();
	memcpy(id,temp.username,18);
	printf("请输入密码    ：");
	getpasswd(temp.userpasswd,12);
	putchar('\n');
	char buff[MAXLINE] = {0};
	MsgHead head;
	head.msglen = sizeof(MsgHead) + sizeof(RegInfo);
	head.msgtype = type;
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);
	memcpy(buff,&head,sizeof(MsgHead));
	memcpy(buff+sizeof(MsgHead),&temp,sizeof(RegInfo));
	ret = write(fd,buff,ntohl(head.msglen));
	if(ret < 0)
	{
		printf("write fail\n");
		return -1;
	}
	else if(ret == 0)
	{
		printf("对端关闭");
		return 1;
	}
	return 0;

}
int  QueryACinemaMovie(MYSQL *pdb,int fd,int type,char *name)
{
	int ret;
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from %s;",name);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("query a cinema movie select * error");
		send_msg(fd,type,3,NULL,0);
		return -1;
	}
	MYSQL_RES * res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,type,1,NULL,0);
		return 0;
	}
	Mov *pmov = (Mov *)malloc(rows * sizeof(Mov));
	memset(pmov,0,rows * sizeof(Mov));
	int i;
	MYSQL_ROW result;
	for(i = 0;i < rows;i++)
	{
		result = mysql_fetch_row(res);
		memcpy((pmov+i)->cinema,name,20);
		memcpy((pmov+i)->name,result[1],24);
		memcpy((pmov+i)->date,result[2],12);
		memcpy((pmov+i)->time,result[3],12);
		(pmov+i)->price = atoi(result[4]);
		(pmov+i)->seatleft = atoi(result[5]);
	}
	send_msg(fd,type,0,pmov,rows*sizeof(Mov));
	mysql_free_result(res);
	return 0;
}
