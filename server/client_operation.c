#include "../inc/client_operation.h"

void *ClientRegister(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	RegInfo temp;
	memset(&temp,0,sizeof(RegInfo));
	memcpy(&temp,p+sizeof(int),sizeof(RegInfo));

	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from UserList where userid = \'%s\'",temp.username);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("register select * error");
		send_msg(fd,2,1,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)		//没有相同的用户名
	{
		memset(buff,0,MAXLINE);
		sprintf(buff,"insert into UserList(userid,userpasswd,telphone,birthdata) values(\'%s\',\'%s\',\'%s\',\'%s\');",temp.username,temp.userpasswd,temp.phonenumber,temp.birthdate);
		ret = mysql_query(pdb,buff);
		if(ret != 0)
		{
			send_msg(fd,2,1,NULL,0);
			perror("register insert error");
			mysql_free_result(res);
			return NULL;
		}
		ret = send_msg(fd,2,0,NULL,0);
		if(ret != 0)
		{
			perror("register send msg 0 error");
			return NULL;
		}
		printf("用户 %s 注册成功\n",temp.username);
	}
	else
	{
		printf("用户名已存在\n");
		ret = send_msg(fd,2,2,NULL,0);
		if(ret != 0)
		{
			perror("register send msg 2 error");
		}
	}
}
void *ClientLogin(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	p = p+sizeof(int);
	RegInfo temp;
	memset(&temp,0,sizeof(RegInfo));
	memcpy(&temp,p,sizeof(RegInfo));

	char buff[MAXLINE] = {0};
	sprintf(buff,"select userpasswd from UserList where userid = \'%s\';",temp.username);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("Client login select error");
		send_msg(fd,4,4,NULL,0);
		return NULL;
	}
	MYSQL_RES* res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,4,1,NULL,0);
		printf("用户名 %s 不存在\n",temp.username);
		mysql_free_result(res);
		return NULL;
	}
	else if(rows == 1)
	{
		MYSQL_ROW result;
		result = mysql_fetch_row(res);
		if(strcmp(result[0],temp.userpasswd))
		{
			send_msg(fd,4,2,NULL,0);
			printf("密码错误\n");
		}
		else
		{
			send_msg(fd,4,0,NULL,0);
			printf("%s 登陆成功\n",temp.username);
		}
		mysql_free_result(res);
		return NULL;
	}
	else
	{
		send_msg(fd,4,3,NULL,0);
		perror("several same id");
		mysql_free_result(res);
		return NULL;
	}
}

void *QueryAllCinemaMovie(void *arg)
{
	int ret;
	int fd = *(int *)arg;

	char buff[MAXLINE] = {0};
	sprintf(buff,"select name from CinemaList;");
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("query all cinema movie select name error");
		send_msg(fd,6,3,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,6,5,NULL,0);
		printf("未找到电影院\n");
		return NULL;
	}
	int i;
	MYSQL_ROW result;
	for(i = 0;i < rows;i++)
	{
		result = mysql_fetch_row(res);
		ret = QueryACinemaMovie(pdb,fd,6,result[0]);
		if(ret != 0)
		{
			send_msg(fd,6,4,result[0],strlen(result[0]));
			printf("电影院 %s 查找失败\n",result[0]);
		}
	}
	mysql_free_result(res);
	return NULL;
}
void *ClientQueryACinemaMovie(void *arg)
{
	int ret;
	char *p = (char *)arg;
	int fd = (int )*p;
	p = p+sizeof(int);
	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",p);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("client query a cinema movie select error");
		send_msg(fd,6,3,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,6,2,NULL,0);
		printf("不存在该影院\n");
		return NULL;
	}
	ret = QueryACinemaMovie(pdb,fd,6,p);
	if(ret != 0)
	{
		printf("查询错误\n");
		return NULL;
	}
	return NULL;
}
void *ClientQueryAMovie(void *arg)
{
	int ret;
	char* p =(char*)arg;
	int fd = (int)*p;
	p = p + sizeof(int);
	char buff[MAXLINE] = {0};
	sprintf(buff,"select name from CinemaList;");
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("query a movie select name error");
		send_msg(fd,6,3,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,6,5,NULL,0);
		printf("未找到电影院\n");
		return NULL;
	}
	int i;
	MYSQL_ROW result;
	for(i = 0;i < rows;i++)
	{
		result = mysql_fetch_row(res);
		memset(buff,0,MAXLINE);
		sprintf(buff,"select * from %s where 电影名 = \'%s\';",result[0],p);
		ret = mysql_query(pdb,buff);
		if(ret != 0)
		{
			printf("电影院 %s 查询失败\n",result[0]);
			send_msg(fd,6,4,result[0],strlen(result[0]));
			continue;
		}
		MYSQL_RES *r = mysql_store_result(pdb);
		int ro = mysql_num_rows(r);
		if(ro == 0)
			continue;
		Mov *p = (Mov*)malloc(ro * sizeof(Mov));
		MYSQL_ROW re;
		int j;
		for(j = 0;j < ro;j++)
		{
			re = mysql_fetch_row(r);
			memcpy((p+j)->cinema,result[0],20);
			memcpy((p+j)->name,re[1],24);
			memcpy((p+j)->date,re[2],12);
			memcpy((p+j)->time,re[3],12);
			(p+j)->price = atoi(re[4]);
			(p+j)->seatleft = atoi(re[5]);
		}
		send_msg(fd,6,0,p,sizeof(Mov)*ro);
	}
	mysql_free_result(res);
	return NULL;
}

void *ClientOrderTicket(void *arg)
{
	int ret;
	char *p =(char *)arg;
	int fd = *(int *)p;
	int num = *(int *)(p + sizeof(int));
	num = ntohl(num);
	char temp[2][24] = {0};
	memcpy(&temp,p+2*sizeof(int),sizeof(temp));

	char buff[MAXLINE] = {0};
	sprintf(buff,"select * from CinemaList where name = \'%s\';",temp[0]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("Client order ticket select cinema error");
		send_msg(fd,12,3,NULL,0);
		return NULL;
	}
	MYSQL_RES *res = mysql_store_result(pdb);
	int rows = mysql_num_rows(res);
	mysql_free_result(res);
	if(rows == 0)
	{
		send_msg(fd,12,4,NULL,0);
		return NULL;
	}

	pthread_mutex_lock(&order_ticket_lock);

	memset(buff,0,MAXLINE);
	sprintf(buff,"select 剩余座位 from %s where 电影名 = \'%s\';",temp[0],temp[1]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("client order ticket select seat error");
		send_msg(fd,12,3,NULL,0);
		return NULL;
	}
	res = mysql_store_result(pdb);
	rows = mysql_num_rows(res);
	if(rows == 0)
	{
		send_msg(fd,12,5,NULL,0);
		mysql_free_result(res);
		pthread_mutex_unlock(&order_ticket_lock);
		return NULL;
	}
	MYSQL_ROW result = mysql_fetch_row(res);
	int total = atoi(result[0]);
	printf("余票 %d\n",total);
	if(total  == 0)
	{
		send_msg(fd,12,1,NULL,0);
		mysql_free_result(res);
		pthread_mutex_unlock(&order_ticket_lock);
		return NULL;
	}
	else if(total < num)
	{
		send_msg(fd,12,2,NULL,0);
		mysql_free_result(res);
		pthread_mutex_unlock(&order_ticket_lock);
		return NULL;
	}
	memset(buff,0,MAXLINE);
	sprintf(buff,"update  %s set 剩余座位 = %d where 电影名 = \'%s\';",temp[0],total-num,temp[1]);
	ret = mysql_query(pdb,buff);
	if(ret != 0)
	{
		perror("update 剩余座位");
		send_msg(fd,12,3,NULL,0);
		mysql_free_result(res);
		pthread_mutex_unlock(&order_ticket_lock);
		return NULL;
	}
	send_msg(fd,12,0,NULL,0);
	mysql_free_result(res);
	pthread_mutex_unlock(&order_ticket_lock);
	return NULL;
}
