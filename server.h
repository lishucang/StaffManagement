#ifndef __SERVER_H__
#define __SERVER_H__

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define  SQLITEPATH 		"staff_manage_system.db"
#define  NAMELEN 	 		32
#define  ADMIN 				0
#define  USER 				1
#define  PASSWORDLEN 		20
#define  ADMIN_QUERY 		0x02
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

int do_client(int acceptfd, sqlite3 *db);
int do_login_request(int acceptfd, MSG *msg , sqlite3 *db);
int process_client_request(int acceptfd,MSG *msg,sqlite3 *db);
void client_admin_query_name(int acceptfd,MSG *msg,sqlite3 *db);
void client_admin_query_all(int acceptfd,MSG *msg,sqlite3 *db);






#endif   //__SERVER_H__
