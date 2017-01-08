#include "../inc/admin.h"

int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr,"Usage:%s IP Port\n",argv[0]);
		return 1;
	}
	//**************************************************
	//连接服务器
	//1 创建套接字
	int sockfd,ret;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket fail");
		return -1;
	}
	//2 连接服务器
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

	//3 创建新线程用于接受来自服务器的消息
	pthread_t pid;
	ret = pthread_create(&pid,NULL,handler,&sockfd);
	if(ret == -1)
	{
		perror("pthread create");
		close(sockfd);
		return -1;
	}

	while(1)
	{
		printf("\n\
				欢迎使用电影院购票系统\n\
				\t管理员版\n\
				1、用户注册\n\
				2、用户登陆\n\
				3、退出界面\n\
				请选择：");
		int input;
		scanf("%d",&input);
		switch(input)
		{
			case 1:ret = Register(sockfd,101);
				   break;
			case 2:ret = LogIn(sockfd,103);
				   while(login_success_flag == 0);
				   if(login_success_flag == 1)	//登陆成功
				   {
					   login_success_flag = 0;
					   admin_operate(sockfd);
				   }
				   else							//登陆失败
				   {
					   login_success_flag = 0;
				   }
				   break;
			case 3:return 0;
			default:printf("你选择的选项有误！\n");
		}
	}
	pthread_join(pid,NULL);
	close(sockfd);
	return 0;
}
int admin_operate(int fd)
{
	while(1)
	{
		printf("\n\
				管理员%s，欢迎您\n\
				1 管理电影院\n\
				2 管理影片\n\
				3 退出登陆\n\
				请选择：",id);
		int input;
		scanf("%d",&input);
		switch(input)
		{
			case 1:operate_cinema(fd);
				   break;
			case 2:operate_movie(fd);
				   break;
			case 3:return 0;
			default:printf("\n你选择的选项错误\n");
		}
	}
}

int operate_movie(int fd)
{
	int input;
	while(1)
	{
		printf("\n\
				1 添加影片上映\n\
				2 查看指定影院影片信息\n\
				3 修改影片上映信息\n\
				4 下架影片\n\
				5 返回上一层\n\
				请选择：");
		scanf("%d",&input);
		switch(input)
		{
			case 1:add_movie(fd);
				   break;
			case 2:query_cinema_movie(fd);
				   break;
			case 3:update_cinema_movie(fd);
				   break;
			case 4:delete_cinema_movie(fd);break;
			case 5:return 0;
			defalut:printf("\n您选择的选项有误\n");
		}
	}
}
int update_cinema_movie(int fd)
{
	int ret;
	char temp[4][24] = {0};
	printf("请输入要修改的那个电影院的名称：");
	scanf("%s",temp[0]);
	printf("请输入要修改的电影名：");
	scanf("%s",temp[1]);
	printf("请输入要修改哪项信息：");
	scanf("%s",temp[2]);
	printf("请输入新信息：");
	scanf("%s",temp[3]);
	ret = send_to_server(fd,119,(char *)temp,sizeof(temp));
	for(int i=0;i<4;i++)printf("%s\n",temp[i]);
	if(ret != 0)
	{
		perror("update cinema movie write error");
		return -1;
	}
	return 0;

}
int delete_cinema_movie(int fd)
{
	int ret;
	char temp[2][24] = {0};
	printf("请输入要删除的那个电影院的名称：");
	scanf("%s",temp[0]);	//temp[0]保存电影院名称
	printf("请输入要删除的电影名：");
	scanf("%s",temp[1]);	//temp[1]保存电影片名称
	ret = send_to_server(fd,117,(char *)temp,sizeof(temp));
	if(ret != 0)
		return -1;
	return 0;
}
int query_cinema_movie(int fd)
{
	int ret;
	char temp[24] = {0};
	printf("请输入要查看的电影院名称：");
	scanf("%s",temp);
	ret = send_to_server(fd,115,temp,sizeof(temp));
	if(ret != 0)
	{
		perror("query cinema write");
		return -1;
	}
	return 0;
}
int add_movie(int fd)
{
	int ret;
	Mov temp;
	memset(&temp,0,sizeof(Mov));
	printf("请输入要添加影片的电影院：\n");
	scanf("%s",temp.cinema);
	printf("请输入影片名：\n");
	scanf("%s",temp.name);
	printf("请输入上映日期（2017-01-01）：\n");
	scanf("%s",temp.date);
	printf("请输入上映时间（08:30:00）：\n");
	scanf("%s",temp.time);
	printf("请输入票价信息：\n");
	scanf("%d",&temp.price);
	printf("请输入剩余座位：\n");
	scanf("%d",&temp.seatleft);
	
	MsgHead head;
	memset(&head,0,sizeof(MsgHead));
	head.msgtype = 113;
	head.msglen = sizeof(MsgHead) + sizeof(Mov);
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);

	char buff[MAXLINE] = {0};
	memcpy(buff,&head,sizeof(MsgHead));
	memcpy(buff+sizeof(MsgHead),&temp,sizeof(Mov));
	ret = write(fd,buff,ntohl(head.msglen));
	if(ret <= 0)
	{
		perror("add movie write");
		return -1;
	}
	return 0;
}

int operate_cinema(int fd)
{
	int input;
	while(1)
	{
		printf("\n\
				1 添加电影院\n\
				2 查询所有电影院信息\n\
				3 修改电影院信息\n\
				4 删除电影院\n\
				5 返回上一层\n\
				请选择：");
		scanf("%d",&input);
		switch(input)
		{
			case 1:add_cinema(fd);
				   break;
			case 2:query_cinema(fd);
				   break;
			case 3:update_cinema(fd);
				   break;
			case 4:delete_cinema(fd);
				   break;
			case 5:return 0;
			default:printf("\n您选择的选项错误\n");
		}
	}
}
int update_cinema(int fd)
{
	int i;
	int ret;
	char temp[3][24] = {0};
	printf("请依次输入 要修改的电影院名称 要修改的栏目(名称，地址，电话) 新值\n");
	for(i = 0;i < 3;i++)
		scanf("%s",temp[i]);
	MsgHead head;
	memset(&head,0,sizeof(MsgHead));
	head.msgtype = 111;
	head.msglen = sizeof(MsgHead) + sizeof(temp);
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);

	char buff[MAXLINE] = {0};
	memcpy(buff,&head,sizeof(MsgHead));
	memcpy(buff+sizeof(MsgHead),temp,sizeof(temp));

	ret = write(fd,buff,ntohl(head.msglen));
	if(ret < 0)
	{
		perror("update cinema write");
		return -1;
	}
	return 0;
}
int delete_cinema(int fd)
{
	int ret;
	char name[20] = {0};
	printf("请输入要删除的电影院名称：");
	scanf("%s",name);

	MsgHead head;
	memset(&head,0,sizeof(MsgHead));
	head.msglen = sizeof(MsgHead) + 20;
	head.msgtype = 109;
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);

	char buff[MAXLINE] = {0};
	memcpy(buff,&head,sizeof(MsgHead));
	memcpy(buff + sizeof(MsgHead),name,20);
	ret = write(fd,buff,ntohl(head.msglen));
	if(ret < 0)
	{
		perror("delete cinema write");
		return -1;
	}
	return 0;
}
int query_cinema(int fd)
{
	int ret;
	MsgHead head;
	memset(&head,0,sizeof(MsgHead));
	head.msglen = sizeof(MsgHead);
	head.msgtype = 107;
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);

	char buff[MAXLINE] = {0};
	memcpy(buff,&head,sizeof(MsgHead));
	ret = write(fd,buff,sizeof(MsgHead));
	if(ret < 0)
	{
		perror("update cinema write");
		return -1;
	}
	return 0;
}
int add_cinema(int fd)
{
	int ret;
	Cin temp;
	memset(&temp,0,sizeof(Cin));
	printf("请输入电影院名称：");
	scanf("%s",temp.name);
	printf("请输入电影院地址：");
	scanf("%s",temp.address);
	printf("请输入电影院电话：");
	scanf("%s",temp.telphone);

	MsgHead head;
	memset(&head,0,sizeof(MsgHead));
	head.msglen = sizeof(MsgHead)+sizeof(Cin);
	head.msgtype = 105;
	head.msglen = htonl(head.msglen);
	head.msgtype = htonl(head.msgtype);

	char buff[MAXLINE] = {0};
	memcpy(buff,&head,sizeof(MsgHead));
	memcpy(buff+sizeof(MsgHead),&temp,sizeof(Cin));
	ret = write(fd,buff,ntohl(head.msglen));
	if(ret <= 0)
	{
		perror("add cinema write");
		return -1;
	}
	return 0;
}

void *handler(void *arg)
{
	int fd =*(int *)arg;
	char buff[MAXLINE] ={0};
	MsgHead temp;
	int ret;
	int value;
	while(1)
	{
		memset(buff,0,MAXLINE);
		ret = read(fd,buff,MAXLINE);
		if(ret <= 0)
		{
			perror("read fd");
			close(fd);
			return NULL;
		}
		memset(&temp,0,sizeof(MsgHead));
		memcpy(&temp,buff,sizeof(MsgHead));
		temp.msglen = ntohl(temp.msglen);
		temp.msgtype = ntohl(temp.msgtype);
		memcpy(&value,buff+sizeof(MsgHead),sizeof(int));
		value = ntohl(value);

		if(temp.msgtype == 102)
		{
			if(value == 0)
			{
				printf("\n注册成功！\n");
				continue;
			}
			else if(value == 1)
			{
				printf("\n注册失败！\n");
				continue;
			}
			else if(value == 2)
			{
				printf("\n用户名已存在！\n");
			}
		}
		else if(temp.msgtype == 104)
		{
			if(value == 0)
			{
				printf("\n登陆成功！\n");
				login_success_flag = 1;		//标志为1,表示登陆成功
				continue;
			}
			else if(value == 1)
			{
				printf("\n用户名不存在！\n");
				login_success_flag = 2;		//标志为2,表示登陆失败
				continue;
			}
			else if(value == 2)
			{
				printf("\n密码错误！\n");
				login_success_flag = 2;
			}
			else if(value == 3)
			{
				printf("\n未知错误！\n");
				login_success_flag = 2;
			}
		}
		else if(temp.msgtype == 106)
		{
			if(value == 0)
			{
				printf("\n添加电影院成功！\n");
				continue;
			}
			else if(value == 1)
			{
				printf("\n电影院名称已存在！\n");
				continue;
			}
			else if(value == 2)
			{
				printf("\nunknown error\n");
				continue;
			}
		}
		else if(temp.msgtype == 108)
		{
			if(value == 0)
			{
				int size = temp.msglen - sizeof(MsgHead) - sizeof(int);
				//printf("size = %d\n",size);
				if(temp.msglen > MAXLINE)
					printf("The returned message is too long!\nYou need to modify #define MAXLINE > %d\n",temp.msglen);
				Cin *pcin = (Cin *)malloc(size);
				memset(pcin,0,size);
				memcpy(pcin,buff+sizeof(MsgHead)+sizeof(int),size);
				int i = 0;
				putchar('\n');
				printf("%-20s%-24s%-12s\n","电影院名称","地址","联系电话");
				for(;i < (size/sizeof(Cin));i++)
				{
					printf("%-20s",(pcin +i)->name);
					printf("%-24s",(pcin +i)->address);
					printf("%-12s\n",(pcin +i)->telphone);
				}
				free(pcin);
			}
			else if(value == 1)
				printf("\n错误\n");
		}
		else if(temp.msgtype == 110)
		{
			if(value == 0)
				printf("\n删除成功\n");
			else if(value == 1)
				printf("\n未找到该影院\n");
			else if(value == 1)
				printf("\n删除失败\n");
		}
		else if(temp.msgtype == 112)
		{
			if(value == 0)
				printf("\n更新信息成功\n");
			else if(value == 1)
				printf("\n电影院选择错误\n");
			else if(value == 2)
				printf("\n栏目选择错误\n");
			else if(value == 3)
				printf("\n未知错误\n");
		}
		else if(temp.msgtype == 114)
		{
			if(value == 0)
				printf("\n添加影片成功\n");
			if(value == 1)
				printf("\n电影院名称错误\n");
			if(value == 2)
				printf("\n未知错误\n");
		}
		else if(temp.msgtype == 116)
		{
			if(value == 0)
			{
				int size = temp.msglen - sizeof(MsgHead) - sizeof(int);
				if(temp.msglen > MAXLINE)
					printf("The returned message is too long!\nYou need to modify #define MAXLINE > %d\n",temp.msglen);
				Mov *pmov = (Mov *)malloc(size);
				memset(pmov,0,size);
				memcpy(pmov,buff+sizeof(MsgHead)+sizeof(int),size);
				int i = 0;
				putchar('\n');
				printf("%-18s%-24s%-15s%-15s%-14s%-s\n","电影院名称","影片","上映日期","上映时间","票价","剩余座位数");
				for(;i < (size/sizeof(Mov));i++)
				{
					printf("%-18s",(pmov +i)->cinema);
					printf("%-20s",(pmov +i)->name);
					printf("%-15s",(pmov +i)->date);
					printf("%-15s",(pmov +i)->time);
					printf("%-7d",(pmov +i)->price);
					printf("%-d\n",(pmov +i)->seatleft);
				}
				free(pmov);
			}
			else if(value == 1)
			{
				printf("\n该电影院没有上映影片\n");
			}
			else if(value == 2)
			{
				printf("\n你输入的电影院名称错误\n");
			}
			else if(value == 3)
			{
				printf("\n未知错误\n");
			}
		}
		else if(temp.msgtype == 118)
		{
			if(value == 0)
				printf("\n下架电影成功\n");
			if(value == 1)
				printf("\n电影院名称错误\n");
			if(value == 2)
				printf("\n电影名错误\n");
			if(value == 3)
				printf("\n未知错误\n");
		}
		else if(temp.msgtype == 120)
		{
			if(value == 0)
				printf("\n更新成功\n");
			if(value == 1)
				printf("\n更新失败\n");
		}
	}
}
