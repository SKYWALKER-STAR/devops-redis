/* 
 * **********************************************************
 * Author: ming
 * 创建时间: 2024/02/20
 * 最后修改时间: 2024/02/20
 * 作用: 遍历Redis数据库中所有的键
 * **********************************************************
*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "hiredis.h"

/*
 * *********************************************************
 * @作用:	打印Redis返回信息
 * @reply: 	redisReply类型指针
 * **********************************************************
*/

void showReply(redisReply *reply) 
{
	switch (reply -> type) {
		case REDIS_REPLY_ARRAY:
			printf("Type:REDIS_REPLY_ARRAY\n");
			break;
		case REDIS_REPLY_STRING:
			printf("\n%s\n",reply->str);
			break;
		case REDIS_REPLY_INTEGER:
			printf("Type:REDIS_REPLY_INTEGER\n");
			break;
		default:
			printf("Unkonw return type: %d\n",reply->type);
			break;
		
	}		
}

/*
 * **********************************************************
 * @作用:	连接Redis并进行验证
 * @addresss:	Redis地址
 * @port:	Redis端口
 * @password:	Redis密码
 * @return:	返回值是redisContext类型的指针
 * **********************************************************
*/

redisContext *ConnectAndLogin(char *address,int port,char *password)
{
	redisContext *context = (redisContext *)malloc(sizeof(redisContext));
	context = redisConnect(address,port);
	if (context -> err) {
		redisFree(context);
		printf("connect redisServer err:%s\n",context -> errstr);
		return;
	}

	printf("connect redisServer success\n");

	char *cmd = "auth ";
	strcat(cmd,password);

	redisReply *reply = (redisReply *)redisCommand(context,cmd);
	if (NULL == reply) {
		printf("command execute failure: %s\n",cmd);
		return;
	}

	if (!(reply -> type == REDIS_REPLY_STATUS && strcmp(reply -> str,"OK") == 0)) {
		printf("command execute failure:%s\n",cmd);
		showReply(reply);
		freeReplyObject(reply);
		redisFree(context);
		return;
	}
	freeReplyObject(reply);
	return context;
}

/*
 * **********************************************************
 * @作用:	选择数据库
 * @context:	redisContext类型指针
 * @number:	数据库编号
 * **********************************************************
*/

void *selectDatabase(redisContext *context,int number)
{
	
	char cmd[100] = "select ";
	char dbnumber[10];

	sprintf(dbnumber,"%d",number);

	/* 将数据库编号和select命令组合 */
	strcat(cmd,dbnumber);

	redisReply *reply = (redisReply *)redisCommand(context,cmd);
	if (NULL == reply) {
		printf("command execute failure:%s\n",cmd);
		return;
	}

	if (!(reply -> type == REDIS_REPLY_STATUS && strcmp(reply -> str,"OK") == 0)) {
		printf("command execute failure:%s\n",cmd);
		freeReplyObject(reply);
		redisFree(context);
		return;
	}
	freeReplyObject(reply);
} 

/*
 * **********************************************************
 * @作用:	获取单个键的值
 * @context:	redisContext类型指针
 * @keyName:	要获取的键的名称
 * **********************************************************
*/

void getSingleKey(redisContext *context,char *keyName)
{
	char cmd[10000] = "get ";

	strcat(cmd,keyName);
	redisReply *reply = (redisReply*) redisCommand(context,cmd);
	if (NULL == reply) {
		printf("command execute failure:%s\n",cmd);
		return;
	}

	showReply(reply);
	freeReplyObject(reply);
}

/*
 * **********************************************************
 * @作用:	获取单个键的类型
 * @conetxt:	redisContext类型指针
 * @keyName:	要获取的键的名称
 * **********************************************************
*/

void checkType(redisContext *context,char *keyName)
{
	char cmd[10000] = "type ";

	strcat(cmd,keyName);
	redisReply *reply = (redisReply*) redisCommand(context,cmd);
	if (NULL == reply) {
		printf("command execute failure:%s\n",cmd);
		return;
	}

	showReply(reply);
	freeReplyObject(reply);
}

/*
 * **********************************************************
 * @作用:	当Redis返回数组时，遍历所有的键
 * @replyList:	redisReply数组
 * @elments:	数组中元素的数量
 * **********************************************************
*/

void travalReply(redisContext *context,redisReply **replyList,int elements)
{
	int i;

	i = 0;
	for (i = 0;i < elements;i++) {
		switch (replyList[i] -> type) {
			case REDIS_REPLY_ARRAY:
				printf("Type:REDIS_REPLY_ARRAY\n");
				break;
			case REDIS_REPLY_STRING:
				getSingleKey(context,replyList[i]->str);
				break;
			case REDIS_REPLY_INTEGER:
				printf("Type:REDIS_REPLY_INTEGER\n");
				break;
			default:
				printf("Unkonw return type\n");
				break;
			
		}		
	}

}

/*
 * **********************************************************
 * @作用:	获取redis中所有的键
 * @context:	redisContext类型指针
 * **********************************************************
*/

void getAllKeys(redisContext *context)
{
	char  *cmd = "keys *";
	redisReply *reply = (redisReply*) redisCommand(context,cmd);
	if (NULL == reply) {
		printf("command execute failure:%s\n",cmd);
		return;
	}

	travalReply(context,reply->element,reply->elements);
	freeReplyObject(reply);

}

/*
 * **********************************************************
 * @作用: 入口函数
 * **********************************************************
*/

int main(int argc,char **argv)
{
	if (argc != 3) {
		printf("usage:%s:[address] [port] [password] [[dbnumer]] \n",argv[0]);
		exit(-1);
	}

	char *address = argv[0];
	int port = atoi(argv[1]);
	char *password = argv[2];

	int dbnumber;

	/* 指定数据库编号，默认为0 */
	if (argc == 4) {
		dbnumber = atoi(argv[3]);
	} else {
		dbnumber = 0;
	}

	redisContext *context;

	context = ConnectAndLogin(address,port,password);
	selectDatabase(context,dbnumber);
	getAllKeys(context);
}
