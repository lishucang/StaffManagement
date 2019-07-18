#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define  N  				64
#define  NAMELEN 	 		32
#define  ADMIN 				0
#define  USER 				1
#define  PASSWORDLEN 		20
#define  QUIT 				0x000
#define  ADMIN_LOGIN 		0x001
#define  USER_LOGIN 		0x100
#define  ADMIN_QUERY_NAME 	0x011
#define  ADMIN_QUERY_ALL 	0x012
#define  ADMIN_MODIFICATION 0x020
#define  ADMIN_ADD 			0X030
#define  ADMIN_DELETE 		0x040
#define  ADMIN_HISTORY 		0x050
#define  USER_QUERY 		0x110
#define  USER_MODIFICATION  0x120

//定义通信双方的信息结构体
typedef struct{
	int staffno;
	int usertype;
	char name[NAMELEN];
	char passwd[PASSWORDLEN];
	int age;
	double phone;
	char addr[256];
	char work[256];
	char data[32];
	int leval;
	float salary;
	char recvmsg[128];
	int msgtype;
	int flag;
}MSG;

void do_login(int sockfd,MSG *msg);
int admin_or_user_login(int sockfd,MSG *msg);
void admin_menu(int sockfd,MSG *msg);
void user_menu(int sockfd,MSG *msg);
void do_admin_query(int sockfd,MSG *msg);
void do_admin_query_name(int sockfd,MSG *msg);
void do_admin_query_all(int sockfd,MSG *msg);
void do_admin_modification(int sockfd,MSG *msg);
void do_admin_add(int sockfd,MSG *msg);
void do_admin_delete(int sockfd,MSG *msg);
void do_admin_history(int sockfd,MSG *msg);
void do_user_query(int sockfd,MSG *msg);
void do_user_modification(int sockfd,MSG *msg);


#endif //__CLIENT_H__
