#include "../inc/client.h"

int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr,"Usage:%s IP Port\n",argv[0]);
		fprintf(stdout,"For example:%s 127.0.0.1 8080\n",argv[0]);
		return 1;
	}

	//*********************************************************
	//连接服务器
	//1 创建套接字
	int sockfd,ret;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket fail");
		return -1;
	}
	//连接服务器
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));
	ret = connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
		perror("connect fail");
		close(sockfd);
		return -1;
	}
	//创建新线程用于接受来自服务器的消息
	pthread_t pid;
	ret = pthread_create(&pid,NULL,handler,&sockfd);
	if(ret != 0)
	{
		perror("pthread create");
		close(sockfd);
		return -1;
	}
	while(1)
	{
		printf("\n\
				欢迎使用电影院购票系统\n\
				\t用户版\n\
				1 用户注册\n\
				2 用户登陆\n\
				3 退出界面\n\
				请选择：");
		int input;
		scanf("%d",&input);
		switch(input)
		{
			case 1:ret = Register(sockfd,1);
				break;
			case 2:ret = LogIn(sockfd,3);
				   while(login_success_flag == 0);
				   if(login_success_flag == 1)
				   {
					   login_success_flag = 0;
					   client_operate(sockfd);
				   }
				   else
				   {
					   login_success_flag = 0;
				   }
				   break;
			case 3:
				   return 0;
			default:
				printf("你选择的选项有误！\n");
		}
	}
	pthread_join(pid,NULL);
	close(sockfd);
	return 0;
}
int client_operate(int fd)
{
	int input;
	while(1)
	{
		printf("\n\
				1 电影票查询\n\
				2 下单购票\n\
				3 退出登陆\n\
				请选择:");
		scanf("%d",&input);
		switch(input)
		{
			case 1:query_ticket(fd);break;
			case 2:order_ticket(fd);break;
			case 3:return 0;
			default:printf("\n您选择的选项有误\n");
		}
	}
}
int order_ticket(int fd)
{
	int ret;
	int num;
	char temp[2][24] ={0};
	printf("请输入要订购的电影院名称：");
	scanf("%s",temp[0]);
	printf("请输入要订购的电影名：");
	scanf("%s",temp[1]);
	printf("请输入要订购的数量：");
	scanf("%d",&num);
	num = htonl(num);
	
	char buff[MAXLINE] = {0};
	memcpy(buff,&num,sizeof(int));
	memcpy(buff+sizeof(int),temp,sizeof(temp));
	ret = send_to_server(fd,11,buff,sizeof(int) + sizeof(temp));
	if(ret != 0)
	{
		perror("order ticket write error");
		return -1;
	}
	return 0;
}
int query_ticket(int fd)
{
	int input;
	while(1)
	{
		printf("\n\
				1 查询所有电影院上映信息\n\
				2 查询指定电影院上映信息\n\
				3 查询指定电影上映信息\n\
				4 返回上一层\n\
				请选择:");
		scanf("%d",&input);
		switch(input)
		{
			case 1:query_all_cinema(fd);break;
			case 2:query_a_cinema(fd);break;
			case 3:query_a_movie(fd);break;
			case 4:return 0;
			default:printf("您选择的选项有误");
		}
	}
}
int query_a_movie(int fd)
{
	int ret;
	char name[20] = {0};
	printf("请输入要搜索的电影名:");
	scanf("%s",name);
	ret = send_to_server(fd,9,name,strlen(name));
	if(ret != 0)
	{
		perror("query a cinema write");
		return -1;
	}
	return 0;
}
int query_a_cinema(int fd)
{
	int ret;
	char name[20] = {0};
	printf("请输入要搜索的电影院名称:");
	scanf("%s",name);
	ret = send_to_server(fd,7,name,strlen(name));
	if(ret != 0)
	{
		perror("query a cinema write");
		return -1;
	}
	return 0;
}
int query_all_cinema(int fd)
{
	int ret;
	ret = send_to_server(fd,5,NULL,0);
	if(ret != 0)
	{
		perror("query all cinema write");
		return -1;
	}
	return 0;
}
void *handler(void *arg)
{
	int fd = *(int *)arg;
	char buff[MAXLINE] = {0};
	MsgHead temp;
	int ret;
	int value;
	while(1)
	{
		memset(buff,0,MAXLINE);
		ret = read(fd,buff,MAXLINE);
		if(ret < 0)
		{
			perror("pipe break");
			close(fd);
			return NULL;
		}
		else if(ret == 0)
		{
			perror("pear close");
			close(fd);
			return NULL;
		}
		memset(&temp,0,sizeof(MsgHead));
		memcpy(&temp,buff,sizeof(MsgHead));
		temp.msglen = ntohl(temp.msglen);
		temp.msgtype = ntohl(temp.msgtype);
		memcpy(&value,buff+sizeof(MsgHead),sizeof(int));
		value = ntohl(value);

		if(temp.msgtype == 2)
		{
			if(value == 0)
				printf("\n注册成功\n");
			else if(value == 1)
				printf("\n注册失败\n");
			else if(value == 1)
				printf("\n用户名已存在\n");
		}
		else if(temp.msgtype == 4)
		{
			if(value == 0)
			{
				login_success_flag = 1;
				printf("\n登陆成功\n");
			}
			if(value == 1)
			{
				login_success_flag = 2;
				printf("\n用户名不存在\n");
			}
			if(value == 2)
			{
				login_success_flag = 2;
				printf("\n密码错误\n");
			}
			if(value == 3)
			{
				login_success_flag = 2;
				printf("\n未知错误\n");
			}
		}
		else if(temp.msgtype == 6)
		{
			if(value == 0)
			{//显示
				int size = temp.msglen - sizeof(MsgHead)- sizeof(int);
				if(temp.msglen > MAXLINE)
					printf("The returned message is too long!\nYou need to modify #define MAXLINE > %d\n",temp.msglen);
				Mov *pmov = (Mov *)malloc(size);
				memset(pmov,0,size);
				memcpy(pmov,buff+sizeof(MsgHead)+sizeof(int),size);
				int i = 0;
				putchar('\n');
				printf("%-18s%-24s%-15s%-15s%-14s%-s\n","电影名名称","影片","上映日期","上映时间","票价","剩余座位数");
				for(;i<(size/sizeof(Mov));i++)
				{
					printf("%-18s",(pmov + i)->cinema);
					printf("%-20s",(pmov + i)->name);
					printf("%-15s",(pmov + i)->date);
					printf("%-15s",(pmov + i)->time);
					printf("%-7d",(pmov + i)->price);
					printf("%-d\n",(pmov + i)->seatleft);
				}
				free(pmov);
			}
			else if(value == 1)
			{//没有数据，不需处理
			}
			else if(value == 2)
			{
				printf("\n电影院名称错误\n");
			}
			else if(value == 3)
			{
				printf("\n服务器错误\n");
			}
			else if(value == 4)
			{
				printf("\n电影院 %s 信息查询失败\n",buff+sizeof(MsgHead)+sizeof(int));
			}
			else if(value == 5)
			{
				printf("\n没有电影院信息\n");
			}
		}
		else if(temp.msgtype == 12)
		{
			if(value == 0)
			{
				printf("\n订票成功\n");
			}
			if(value == 1)
			{
				printf("\n无票\n");
			}
			if(value == 2)
			{
				printf("\n余票不足\n");
			}
			if(value == 3)
			{
				printf("\n未知错误\n");
			}
			if(value == 4)
			{
				printf("\n未找到该电影院\n");
			}
			if(value == 5)
			{
				printf("\n该电影院无此电影上映\n");
			}
		}

	}
}
