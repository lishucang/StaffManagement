#include "client.h"

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	char buf[128] = {0};
	MSG msg;

	if(argc != 3){
		printf("Usage:%s serverip port.\n",argv[0]);
		return -1;
	}

	if(0 > (sockfd = socket(AF_INET,SOCK_STREAM,0))){
		perror("failed to socket.\n");
		return -1;
	}
	
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

	if(0 < connect(sockfd, (struct sockaddr *)&serveraddr,sizeof(serveraddr))){
		perror("failed to connect.\n");
		return -1;
	}
	
	while(1)
	{
		do_login(sockfd,&msg);

		fgets(buf,sizeof(buf),stdin);
		buf[strlen(buf)-1] = '\0';
		
		send(sockfd,buf,sizeof(buf),0);

		if(strncmp(buf,"quit",4) == 0)
		{
			printf("client is exit.\n");
			break;
		}				
	}
	close(sockfd);
	return 0;
}

/*****************************************
 *功能:登录模式选择
 *参数:sockfd  套接字
 	   *msg     消息结构体
 *返回值:无	   
 * ***************************************/
void do_login(int sockfd,MSG *msg)
{
	int n;
	while(1){
		printf("*********************************************************\n");
		printf("*    1：管理员模式     2：普通用户模式     3：退出      *\n");
		printf("*********************************************************\n");
		printf("请输入您的选择(数字)>>>");
		scanf("%d",&n);
		getchar();

		switch(n){
		case 1:
			msg->msgtype = ADMIN_LOGIN;
			msg->usertype = ADMIN;
			admin_or_user_login(sockfd,msg);
			break;
		case 2:
			msg->usertype = USER;
			msg->msgtype = USER_LOGIN;
			admin_or_user_login(sockfd,msg);
			break;
		case 3:
			msg->msgtype = QUIT;
			send(sockfd,&msg,sizeof(MSG),0);
			close(sockfd);
			exit(0);
		default:
			printf("输入有误,请重新输入.\n");
		}
	}
}
/*************************************************
 * 功能:登录验证
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:登录成功返回0.失败-1
 *************************************************/
int admin_or_user_login(int sockfd,MSG *msg)
{
	printf("-------------------%s-------------------\n",__func__);
	//输入用户名和密码
	memset(msg->name,0,NAMELEN);
	printf("请输入用户名:");
	scanf("%s",msg->name);
	getchar();

	memset(msg->passwd,0,PASSWORDLEN);
	printf("请输入密码:");
	scanf("%s",msg->passwd);
	getchar();

	//发送登录请求
	send(sockfd,msg,sizeof(MSG),0);
	//接收服务器响应
	if(recv(sockfd,msg,sizeof(MSG),0)<0){
		perror("failed to recv.\n");
		return -1;
	}
	printf("msg->recvmsg:%s\n",msg->recvmsg);

	//判断是否登录成功
	if(strncmp(msg->recvmsg,"OK",2) == 0){
		if(msg->usertype == ADMIN){
			printf("亲爱的管理员,欢迎您登录员工管理系统!\n");
			admin_menu(sockfd,msg);			
		}
		else if(msg->usertype == USER){
			printf("亲爱的用户,欢迎您登录员工管理系统!\n");
			user_menu(sockfd,msg);
		}
	}
	else{
		printf("登录失败!%s\n",msg->recvmsg);
		return -1;
	}
	return 0;
}

/*************************************************
 * 功能:管理员菜单管理
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void admin_menu(int sockfd,MSG *msg)
{
	printf("-----------------------%s------------------------\n",__func__);
	int n;
	while(1)
	{
		printf("*********************************************************\n");
		printf("*  1:查询          2:修改              3:添加用户       *\n");
		printf("*  4:删除用户      5:查询历史记录      6:退出 	    	*\n");
		printf("*********************************************************\n");
		printf("请输入您的选择(数字)>>>");
		scanf("%d",&n);
		getchar();

		switch(n){
		case 1:
			do_admin_query(sockfd,msg);
			break;
		case 2:
			do_admin_modification(sockfd,msg);
			break;
		case 3:
			do_admin_add(sockfd,msg);
			break;
		case 4:
			do_admin_delete(sockfd,msg);
			break;
		case 5:
			do_admin_history(sockfd,msg);
			break;
		case 6:
			msg->msgtype = QUIT;
			send(sockfd,msg,sizeof(MSG),0);
			close(sockfd);
			exit(0);
		default:
			printf("您输入有误,请重新输入！\n");
		}						
	}
}

/*************************************************
 * 功能:管理员查询员工管理系统信息
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_query(int sockfd,MSG *msg)
{
	printf("---------------------%s----------------------\n",__func__);
	int n;
	while(1)
	{
		printf("*********************************************************\n");
		printf("*  1:按人名查找       2:查找所有          3:退出        *\n");
		printf("*********************************************************\n");
		printf("请输入您的选择(数字)>>>");
		scanf("%d",&n);
		getchar();

		switch(n){
		case 1:
			do_admin_query_name(sockfd,msg);
			break;
		case 2:
			do_admin_query_all(sockfd,msg);
			break;
		case 3:
			admin_menu(sockfd,msg);
			break;
		default:
			printf("输入错误,请重新输入!\n");
		}
	}
}

/*************************************************
 * 功能:管理员按人名查询员工管理系统信息
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_query_name(int sockfd,MSG *msg)
{			
	printf("请输入你要查找的用户名:");
	scanf("%s",msg->name);
	getchar();
	msg->msgtype = ADMIN_QUERY_NAME;
	send(sockfd,msg,sizeof(MSG),0); //向服务器发送查询信息
	while(1){
		recv(sockfd,msg,sizeof(MSG),0);
		if(strncmp(msg->recvmsg,"OK",2) == 0){
			printf("%d %d %s %s %d %f %s %s %s %d %f\n",\
					msg->staffno,msg->usertype,msg->name,msg->passwd,msg->age,msg->phone,msg->addr,msg->work,msg->data,msg->leval,msg->salary);
		}
		else{
			break;
		}
	}

	msg->flag=0;
}


/*************************************************
 * 功能:管理员查询所有员工信息
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_query_all(int sockfd,MSG *msg)
{			
	msg->msgtype = ADMIN_QUERY_ALL;
	send(sockfd,msg,sizeof(MSG),0); //向服务器发送查询信息
	while(1){
		recv(sockfd,msg,sizeof(MSG),0);
		if(msg->flag !=2){
			printf("%s\n",msg->recvmsg);
		}
		else{
			break;
		}
	}
	msg->flag=0;
}

/*************************************************
 * 功能:管理员修改管理系统信息
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_modification(int sockfd,MSG *msg)
{
	printf("-----------------%s-------------------\n",__func__);
	
}

/*************************************************
 * 功能:管理员添加员工管理系统信息
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_add(int sockfd,MSG *msg)
{
	printf("----------------------%s-----------------------\n",__func__);
	
}

/*************************************************
 * 功能:管理员删除员工管理系统信息
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_delete(int sockfd,MSG *msg)
{
	printf("--------------------%s----------------------\n",__func__);
	
}

/*************************************************
 * 功能:管理员查询员工管理系统信息的操作历史记录
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_admin_history(int sockfd,MSG *msg)
{
	printf("--------------------%s---------------------\n",__func__);
	
}

/*************************************************
 * 功能:员工菜单管理
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void user_menu(int sockfd,MSG *msg)
{
	printf("------------------------%s------------------------\n",__func__);
	int n;
	while(1)
	{
		printf("*********************************************************\n");
		printf("*  1:查询               2:修改            3:退出        *\n");
		printf("*********************************************************\n");
		printf("请输入您的选择(数字)>>>");
		scanf("%d",&n);
		getchar();

		switch(n){
		case 1:
			do_user_query(sockfd,msg);
			break;
		case 2:
			do_user_modification(sockfd,msg);
			break;
		case 3:
			msg->msgtype = QUIT;
			send(sockfd, msg, sizeof(MSG),0);
			close(sockfd);
			exit(0);
		default:
			printf("您输入有误，请重新输入!\n");
		}
	}
}

/*************************************************
 * 功能:员工查询
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_user_query(int sockfd,MSG *msg)
{
	printf("---------------------%s-----------------------\n",__func__);

}

/*************************************************
 * 功能:员工修改
 * 参数:sockfd  套接字	
 * 	   *msg    消息结构体
 * 返回值:空
 * ***********************************************/
void do_user_modification(int sockfd,MSG *msg)
{
	printf("-----------------%s--------------------\n",__func__);

}





