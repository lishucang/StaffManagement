#include "server.h"

/***********************************
 * 功能:主函数
 * **********************************/
int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int acceptfd;
	pid_t pid;
	sqlite3 *db;
	char buf[128] = {0};
	ssize_t recvbytes;
	int on = 1;
	MSG msg;

	if(argc != 3){
		printf("Usage:%s serverip  port.\n", argv[0]);
		return -1;
	}

	//打开数据库
	if(sqlite3_open(SQLITEPATH,&db) != SQLITE_OK){
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	
	//创建套接字并判断是否成功连接
	if(0>(sockfd = socket(AF_INET,SOCK_STREAM,0))){
		perror("failed to socket.\n");
		return -1;
	}
	//允许重用本地IP和端口
	if (0 > setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
	{
		perror("failed tosetsockopt.\n");
		return 	-1;
	}
	
	bzero(&serveraddr,sizeof(serveraddr));  			//将内存区数据清0
	serveraddr.sin_family = AF_INET; 					//设置IPV4协议族
	serveraddr.sin_port = htons(atoi(argv[2])); 		//设置端口号
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]); 	//设置IP地址
	//绑定套接字
	if(0>bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))){
		perror("failed to bind.\n");
		return -1;
	}
	//将套接字设为监听模式
	if(0>listen(sockfd,5)){
		perror("failed to bind.\n");
		return -1;
	}
	//处理僵尸进程
	signal(SIGCHLD,SIG_IGN);


	while(1)
	{
		if(0>(acceptfd = accept(sockfd,NULL,NULL))){
			perror("failed to accept.\n");
			return -1;
		}
		printf("client ip is %s port:%d\n",\
				(char *)inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
	

		if(0 > (pid = fork())){
			perror("failed to fork.\n");
			return -1;
		}
		else if(pid == 0){ 	//子进程,处理客户端具体的消息
			close(sockfd);
			do_client(acceptfd,db);
		}
		else{ 				//父进程,用来接受客户端的请求
			close(acceptfd);
		}
	}

	close(acceptfd);
	close(sockfd);

	return 0;
}

int do_client(int acceptfd, sqlite3 *db)
{
	MSG msg;
	while(recv(acceptfd, &msg, sizeof(MSG), 0) > 0){
		printf("schema type:%d\n", msg.usertype);
		switch(msg.usertype){
		case 0:
		case 1:
			process_client_request(acceptfd,&msg,db);

			break;

		default:
			printf("Invalid schema type.\n");
		}
	}
	printf("client is exit.\n");
	close(acceptfd);
	exit(0);

	return 0;
}

int do_login_request(int acceptfd, MSG *msg , sqlite3 *db)
{
	printf("------------------%s-----------------\n",__func__);
	
	char sql[128] = {0};
	char *errmsg;
	int nrow;
	int ncloumn;
	char **resultp;

	printf("usrtype: %#x-----username: %s---passwd: %s.\n",msg->usertype,msg->name,msg->passwd);
	sprintf(sql,"select * from usrinfo where usertype=%d and name='%s' and passwd='%s';",msg->usertype,msg->name,msg->passwd);

	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
	}
	else{
		if(nrow == 0){ // 密码或者用户名错误
			strcpy(msg->recvmsg,"username or password wrong.\n");
			send(acceptfd,msg,sizeof(MSG),0);
		}
		// 查询成功,数据库中拥有此用户
		else{
			strcpy(msg->recvmsg,"OK");
			if(send(acceptfd,msg,sizeof(MSG),0)<0){
				perror("failed to send");
				return -1;
			}
		}
	}
	return 0;
}

void client_admin_query_name(int acceptfd,MSG *msg,sqlite3 *db)
{
	printf("------------------%s-----------------\n",__func__);
	
	char sql[sizeof(MSG)] = {0};
	char *errmsg;
	int nrow;
	int ncloumn;
	char **resultp;
	int i;

	sprintf(sql,"select * from usrinfo where name='%s';",msg->name);

	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
	}
	else{
		if(nrow == 0){ //用户不存在 
			strcpy(msg->recvmsg,"用户不存在.\n");
			send(acceptfd, msg, sizeof(MSG), 0);
		}
		// 查询成功,数据库中拥有此用户
		else{
			strcpy(msg->recvmsg, "OK");

	//		sprintf(sql,"%d %d %s %s %d %s %s %s %s %d %f\n",\
					msg->staffno,msg->usertype,msg->name,msg->passwd,msg->age,msg->phone,msg->addr,msg->work,msg->data,msg->leval,msg->salary);
			printf("resultp = %s\n",sql);
			send(acceptfd, msg, sizeof(MSG), 0);
		}
	}
}

void client_admin_query_all(int acceptfd,MSG *msg,sqlite3 *db)
{

}

int process_client_request(int acceptfd,MSG *msg,sqlite3 *db)
{
	printf("---------------%s--------------\n",__func__);
	printf("msgtype:%d\n", msg->msgtype);
	switch(msg->msgtype){
	case QUIT:
		break;
	case ADMIN_LOGIN:
	case USER_LOGIN:
		do_login_request(acceptfd,msg,db);
		break;
	case ADMIN_QUERY_NAME:
		client_admin_query_name(acceptfd,msg,db);
		break;
	case ADMIN_QUERY_ALL:
		break;
	case ADMIN_MODIFICATION:
		break;
	case ADMIN_ADD:
		break;
	case ADMIN_DELETE:
		break;
	case ADMIN_HISTORY:
		break;
	case USER_QUERY:
		break;
	case USER_MODIFICATION:
		break;
	default:
		break;
	}
}





