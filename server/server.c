#include"../inc/server.h"

int main(int argc,char* argv[])
{
	int portnumber;
	if( 2 == argc)
	{
		if( (portnumber = atoi(argv[1])) < 0)
		{
			fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
			return 1;
		}
	}
	else
	{
		fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
		return 1;
	}

	int ret;
	//***************************************************************
	//1 创建监听链接
	int listenfd,sockfd,connfd;
	int n;	//保存read返回值
	struct sockaddr_in serveraddr,clientaddr;
	listenfd = socket(AF_INET,SOCK_STREAM,0);	//监听文件描述符
	//2 绑定服务器操作
	memset(&serveraddr,0,sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	char *local_addr = "127.0.0.1";
	inet_aton(local_addr,&(serveraddr.sin_addr));	//主机序转换为网络序
	serveraddr.sin_port = htons(portnumber);
	bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr_in));
	//3 监听
	listen(listenfd,LISTENQ);

	socklen_t clilen = sizeof(struct sockaddr_in);

	//**************************************************************
	//创建epoll服务器实例
	struct epoll_event ev,events[LISTENQ];
	int epfd;	//epoll服务器实例文件描述符
	int nfds;	//epoll wait数量
	epfd = epoll_create(256);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);	//把监听fd加入兴趣列表中

	//***************************************************************
	//连接数据库
	MYSQL db;
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","wenjian","mysql",3306,NULL,0);
	if(pdb == NULL)
	{
		perror("mysql_real_connect");
		close(listenfd);
		close(epfd);
		return -1;
	}
	ret = mysql_query(pdb,"set names utf8;");
	if(ret != 0)
	{
		perror("set names utf8");
		close(listenfd);
		close(epfd);
		mysql_close(pdb);
		return -1;
	}
	ret = mysql_select_db(pdb,"电影院");
	if(ret != 0)
	{
		perror("select database 电影院");
		close(listenfd);
		close(epfd);
		mysql_close(pdb);
		return -1;
	}
	else
		printf("present database is 电影院\n");
//*********************************************************************
	//创建线程池
	pool_init(&pool,5);

//*********************************************************************
	pthread_mutex_init(&order_ticket_lock,NULL);

//*************************************************************************
	//主循环
	int i;
	char line[MAXLINE];
	char buff[MAXLINE];
	MsgHead head;
	while(1)
	{
		nfds = epoll_wait(epfd,events,LISTENQ,500);
		for(i = 0;i < nfds;i++)
		{
			printf("\n*******************\nnfds = %d\n",nfds);
			if(events[i].data.fd == listenfd)	//有新连接
			{
				printf("New connect\n");
				connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&clilen);
				printf("new fd = %d\n",connfd);

				if(connfd < 0)
				{
					perror("connfd < 0");
					close(epfd);
					close(listenfd);
					mysql_close(pdb);
					return -1;
				}
				char *str = inet_ntoa(clientaddr.sin_addr);
				printf("accept a connect from %s port = %d\n",str,ntohs(clientaddr.sin_port));
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
				printf("connect done\n");
			}
			else if(events[i].events & EPOLLIN)
			{
				printf("EPOLLIN\n");
				if( (sockfd = events[i].data.fd) < 0 )
				{
					continue;
				}
				if( (n = read(sockfd, line,MAXLINE)) < 0)
				{
					if(errno == ECONNRESET)	//管道破裂错误码
					{
						close(sockfd);
						continue;
					}
					else
					{
						printf("readline error\n");
					}
				}
				else if(n == 0)	//对端关闭文件描述符，是不是应该从兴趣列表中剔除
				{
					close(sockfd);
					continue;
				}
				//清空消息头结构体，每次都要复用
				memset(&head,0,sizeof(MsgHead));
				//将消息头取出
				memcpy(&head,line,sizeof(head));
				//将网络序转换为主机序
				head.msglen = ntohl(head.msglen);
				head.msgtype = ntohl(head.msgtype);
				//清空传参buff（传给线程池任务链表）
				memset(buff,0,MAXLINE);
				//将对端文件描述符写入buff
				memcpy(buff,&sockfd,sizeof(int));
				//将剩余收到的消息写入buff
				if(n > sizeof(MsgHead))
					memcpy(buff+sizeof(int),line+sizeof(head),n-sizeof(head));
				switch(head.msgtype)
				{
					//客户注册消息类型
					case 1:pool_add_work(&pool,ClientRegister,buff);break;
					//客户登陆消息类型
					case 3:pool_add_work(&pool,ClientLogin,buff);
						   break;
					//客户查询电影院列表消息类型
					case 5:pool_add_work(&pool,QueryAllCinemaMovie,buff);
						   break;
					//客户根据电影片名查询消息类型
					case 7:pool_add_work(&pool,ClientQueryACinemaMovie,buff);
						   break;
					//
					case 9:pool_add_work(&pool,ClientQueryAMovie,buff);
						   break;
					//
					case 11:pool_add_work(&pool,ClientOrderTicket,buff);break;
					//
					case 13:break;
					//
					case 15:break;
					//
					case 17:break;
					//
					case 19:break;
					//
					//管理员注册消息类型
					case 101:pool_add_work(&pool,AdminRegister,buff);
							 break;
					//管理员登陆消息类型
					case 103:pool_add_work(&pool,sLogin,buff);
							 break;
					//管理员添加电影院
					case 105:pool_add_work(&pool,AddCinema,buff);
							 break;
					//管理员查询所有电影院信息
					case 107:pool_add_work(&pool,QueryAllCinema,buff);
							 break;
					//删除一个电影院
					case 109:pool_add_work(&pool,DeleteACinema,buff);
							 break;
					//更新电影院的一个信息
					case 111:pool_add_work(&pool,UpdateACinema,buff);
							 break;
					//添加一部电影上映信息
					case 113:pool_add_work(&pool,AddAMovie,buff);
							 break;
					//查询一个电影院的上映信息
					case 115:pool_add_work(&pool,AdminQueryACinema,buff);
							 break;
					//下架一部电影
					case 117:pool_add_work(&pool,DeleteAMovie,buff);
							 break;
					//修改影片上映信息
					case 119:pool_add_work(&pool,UpdateAMovie,buff);
							 break;
					default:printf("Message type wrong!\n");
				}
			}
		}
	}
	mysql_close(pdb);
	close(listenfd);
	pool_destroy(&pool);
	return 0;
}

