#include "../inc/admin_operation.h"
/*
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
	memcpy(buff+sizeof(MsgHead)+sizeof(int),p,len);
	ret = write(fd,buff,ntohl(temp.msglen));
	if(ret < 0)
	{
		perror("write return");
		return -1;
	}
	if(p != NULL)
		free(p);
	return 0;
}
*/

void *UpdateAMovie(void *arg)
{
	char *p = (char *)arg;
//	for(int i=0;i<96;i++)printf("%d ",*(p+i));
//	printf("\n");
//	write(1,p+4,96);
//	printf("\n");
	int fd = (int)*p;
	char temp[4][24] = {0};
	memcpy(temp,p+sizeof(int),sizeof(temp));
//	printf("%ld\n",sizeof(temp));
//	for(int i = 0;i < 4;i++)printf("%s\n",temp[i]);
	
	char buff[MAXLINE] = {0};
	if(!strcmp(temp[2],"票价") || !strcmp(temp[2],"剩余座位"))
		sprintf(buff,"update %s set %s = %s where 电影名 = \'%s\';",temp[0],temp[2],temp[3],temp[1]);
	else
		sprintf(buff,"update %s set %s = \'%s\' where 电影名 = \'%s\';",temp[0],temp[2],temp[3],temp[1]);
	int ret;
	ret = mysql_query(pdb,buff);
//	printf("buff = %s\n",buff);
	if(ret != 0)
	{
		perror("update a movie update error");
		send_msg(fd,120,1,NULL,0);
		return NULL;
	}
	send_msg(fd,120,0,NULL,0);
	return NULL;
}

void *DeleteAMovie(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int)*p;
	char temp[2][24] = {0};
	memcpy(temp,p+sizeof(int),sizeof(temp));

	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",temp[0]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("Delete a movie select 电影院 error");
		send_msg(fd,118,3,NULL,0);
		return NULL;
	}
	MYSQL_RES * res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,118,1,NULL,0);
		mysql_free_result(res);
		return NULL;
	}
	mysql_free_result(res);

	memset(buff,0,MAXLINE);
	sprintf(buff,"select * from %s where 电影名 = \'%s\';",temp[0],temp[1]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("delete a movie select 电影名 error");
		send_msg(fd,118,3,NULL,0);
		return NULL;
	}
	res = mysql_store_result(pdb);
	rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,118,2,NULL,0);
		mysql_free_result(res);
		return NULL;
	}
	mysql_free_result(res);

	memset(buff,0,MAXLINE);
	sprintf(buff,"delete from %s where 电影名 = \'%s\';",temp[0],temp[1]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("delete a movie delete error");
		send_msg(fd,118,3,NULL,0);
		return NULL;
	}
	send_msg(fd,118,0,NULL,0);
	printf("下架电影%s成功\n",temp[1]);
	return NULL;
}
void *AdminQueryACinema(void * arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int)*p;
	p = p + sizeof(int);
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",p);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("Admin query a cinema select * error");
		send_msg(fd,116,3,NULL,0);
		return NULL;
	}
	MYSQL_RES * res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,116,2,NULL,0);
		mysql_free_result(res);
		return NULL;
	}
	mysql_free_result(res);
	ret = QueryACinemaMovie(pdb,fd,116,p);
	if(ret == 0)
		printf("查询电影院 %s 的所有电影成功\n",p);
	else
		printf("查询信息失败 %s\n",p);
	return NULL;
}
void *AddAMovie(void *arg)
{
	int ret;
	char *p =(char *)arg;
	int fd = (int )*p;
	Mov temp;
	memset(&temp,0,sizeof(Mov));
	memcpy(&temp,p+sizeof(int),sizeof(Mov));
	
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",temp.cinema);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("add a movie select 8 error");
		send_msg(fd,114,0,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		mysql_free_result(res);
		send_msg(fd,114,1,NULL,0);
		return NULL;
	}
	mysql_free_result(res);
	memset(buff,0,MAXLINE);
	sprintf(buff,"insert into %s(电影名,上映日期,上映时间,票价,剩余座位) values(\'%s\',\'%s\',\'%s\',%d,%d);",
			temp.cinema,temp.name,temp.date,temp.time,temp.price,temp.seatleft);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("add a movie insert error");
		send_msg(fd,114,2,NULL,0);
		return NULL;
	}
	send_msg(fd,114,0,NULL,0);
	printf("add movie %s in %s success\n",temp.name,temp.cinema);
	return NULL;
}
void *UpdateACinema(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int)*p;
	char temp[3][24] = {0};
	memcpy(temp,p + sizeof(int),sizeof(temp));

	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",temp[0]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("update a cinema error select * error");
		send_msg(fd,112,3,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	//rows == 0,不存在该电影院
	if(rows == 0)
	{
		mysql_free_result(res);
		send_msg(fd,112,1,NULL,0);
		return NULL;
	}
	char column[10] = {0};
	if(!strcmp(temp[1],"名称"))
		memcpy(column,"name",5);
	else if(!strcmp(temp[1],"地址"))
		memcpy(column,"address",8);
	else if(!strcmp(temp[1],"电话"))
		memcpy(column,"telphone",9);
	else
	{
		send_msg(fd,112,2,NULL,0);
		mysql_free_result(res);
		return NULL;
	}
	memset(buff,0,MAXLINE);
	sprintf(buff,"update CinemaList set %s = \'%s\' where name = \'%s\';",column,temp[2],temp[0]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("update a cinema update error");
		send_msg(fd,112,3,NULL,0);
		mysql_free_result(res);
		return NULL;
	}
	//如果修改了电影院的名称，那么还需要修改对应的表名
	if(!strcmp(column,"name"))
	{
		memset(buff,0,MAXLINE);
		sprintf(buff,"alter table %s rename %s;",temp[0],temp[2]);
		ret = mysql_query(pdb,buff);
		if(ret != 0)
		{
			perror("update a cinema alter table rename");
			send_msg(fd,112,3,NULL,0);
			mysql_free_result(res);
			return NULL;
		}
	}
	send_msg(fd,112,0,NULL,0);
	printf("update success\n");
	mysql_free_result(res);
	return NULL;
}
void *DeleteACinema(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	p = p +sizeof(int);

	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",p);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("Delete a cinema select * error");
		send_msg(fd,110,2,NULL,0);
		return NULL;
	}
	MYSQL_RES* res;
	res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,110,1,NULL,0);
		mysql_free_result(res);
		return NULL;
	}
	memset(buff,0,MAXLINE);
	sprintf(buff,"drop table %s;",p);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("drop table error");
		mysql_free_result(res);
		send_msg(fd,110,2,NULL,0);
		return NULL;
	}
	memset(buff,0,MAXLINE);
	sprintf(buff,"delete from CinemaList where name = \'%s\';",p);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("delete from error");
		mysql_free_result(res);
		send_msg(fd,110,2,NULL,0);
		return NULL;
	}
	send_msg(fd,110,0,NULL,0);
	mysql_free_result(res);
	printf("删除电影院 %s 成功\n",p);
	return NULL;
}

void *QueryAllCinema(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList;");
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("Query all cinema error");
		send_msg(fd,108,1,NULL,0);
		return NULL;
	}
	MYSQL_RES *res;
	res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,108,0,NULL,0);
		return NULL;
	}
	Cin *pcin = (Cin *)malloc(rows * sizeof(Cin));
	memset(pcin,0,rows * sizeof(Cin));
	int i;
	MYSQL_ROW result;
	for(i = 0;i < rows;i++)
	{
		result = mysql_fetch_row(res);
		memcpy((pcin + i)->name,result[1],18);
		memcpy((pcin + i)->address,result[2],24);
		memcpy((pcin + i)->telphone,result[3],12);
	}
	send_msg(fd,108,0,pcin,rows * sizeof(Cin));
	mysql_free_result(res);
	return NULL;
}
void *AddCinema(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	p = p+sizeof(int);
	Cin temp;
	memset(&temp,0,sizeof(Cin));
	memcpy(&temp,p,sizeof(Cin));
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",temp.name);
	ret = mysql_query(pdb,buff);
//	printf("ret = %d\n",ret);
	if(ret != 0)
	{
		perror("AddCiname select error");
		send_msg(fd,106,2,NULL,0);
		return NULL;
	}
	MYSQL_RES *res;
	res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		memset(buff,0,MAXLINE);
		sprintf(buff,"insert into CinemaList(name,address,telphone) values(\'%s\',\'%s\',\'%s\');",
				temp.name,temp.address,temp.telphone);
		ret = mysql_query(pdb,buff);
		if(ret != 0)
		{
			perror("AddCinema insert error");
			send_msg(fd,106,2,NULL,0);
			mysql_free_result(res);
			return NULL;
		}
		memset(buff,0,MAXLINE);
		sprintf(buff,"create table %s(id int not null primary key auto_increment,电影名 varchar(24) not null,上映日期 date not null,\
			上映时间 time not null,票价 int not null,剩余座位 int not null)default charset = utf8;",temp.name);
		ret = mysql_query(pdb,buff);
		if(ret != 0)
		{
			perror("AddCinema create table");
			send_msg(fd,106,2,NULL,0);
			mysql_free_result(res);
			return NULL;
		}
		send_msg(fd,106,0,NULL,0);
		printf("添加电影院成功\n");
		mysql_free_result(res);
		return NULL;
	}
	else
	{
		send_msg(fd,106,1,NULL,0);
		printf("电影院名已存在\n");
		mysql_free_result(res);
		return NULL;
	}
}
void *sLogin(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	p = p+sizeof(int);
	RegInfo temp;
	memset(&temp,0,sizeof(RegInfo));
	memcpy(&temp,p,sizeof(RegInfo));
	char buff[MAXLINE] = {0};
	sprintf(buff,"select adminpasswd from AdminList where adminid = \'%s\';",temp.username);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("sLogin select error");
		send_msg(fd,104,4,NULL,0);
		return NULL;
	}
	MYSQL_RES *res;
	res =mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,104,1,NULL,0);
		printf("用户名不存在\n");
		mysql_free_result(res);
		return NULL;
	}
	else if(rows == 1)
	{
		MYSQL_ROW result;
		result = mysql_fetch_row(res);
		if(strcmp(result[0],temp.userpasswd))
		{
			send_msg(fd,104,2,NULL,0);
			printf("密码错误\n");
		}
		else
		{
			send_msg(fd,104,0,NULL,0);
			printf("登陆成功\n");
		}
		mysql_free_result(res);
		return NULL;
		
	}
	else
	{
		send_msg(fd,104,3,NULL,0);
		printf("未知错误\n");
		mysql_free_result(res);
		return NULL;
	}
}
void *AdminRegister(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd =(int)*p;
	p = p+sizeof(int);
	RegInfo temp;
	memset(&temp,0,sizeof(RegInfo));
	memcpy(&temp,p,sizeof(RegInfo));
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from AdminList where adminid = \'%s\';",temp.username);
	ret = mysql_query(pdb,buff);
//	printf("%s\n",buff);
	if(ret != 0)
	{
		perror("Register select error");
		send_msg(fd,102,1,NULL,0);
		return NULL;
	}
	MYSQL_RES *res;
	res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)	//表明没有用户名相同的用户
	{
		memset(buff,0,MAXLINE);
		sprintf(buff,"insert into AdminList(adminid,adminpasswd,telphone,birthdate) values(\'%s\',\'%s\',\'%s\',\'%s\');",
				temp.username,temp.userpasswd,temp.phonenumber,temp.birthdate);
		ret = mysql_query(pdb,buff);
		if(ret != 0)
		{
			send_msg(fd,102,1,NULL,0);
			perror("insert error");
			mysql_free_result(res);
			return NULL;
		}
		ret = send_msg(fd,102,0,NULL,0);
		if(ret == -1)
		{
			printf("send fail\n");
		}
		printf("用户%s注册成功\n",temp.username);
	}
	else
	{
		printf("用户名已存在\n");
		ret = send_msg(fd,102,2,NULL,0);
		if(ret == -1)
		{
			printf("send fail\n");
		}
		//用户名已存在
	}
}
