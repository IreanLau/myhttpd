#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "sql_connect.h" //sql_connect.h
#include <hiredis/hiredis.h> //sql_connect.h

const std::string _remote_ip = "127.0.0.1";
const std::string _remote_user = "root";
//const std::string _remote_ip = "192.168.253.129";
const std::string _remote_port = "6379";
const std::string _remote_passwd = "1205";
const std::string _remote_db   = "remote_table";

int main()
{
	int content_length = -1;
	char method[1024];
	char query_string[1024];
	char post_data[4096];
	memset(method, '\0', sizeof(method));
	memset(query_string, '\0', sizeof(query_string));
	memset(post_data, '\0', sizeof(post_data));

	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>show student</head>"<<std::endl;
	std::cout<<"<body>"<<std::endl;

	strcpy(method, getenv("REQUEST_METHOD"));
	 if( strcasecmp("POST", method) == 0 )
	 {
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0; 
		for(; i < content_length; i++)
		{
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
	 }
		//std::cout<<"<head>"<<post_data<<"</head>"<<std::endl;

		char name[64];
		char age[64];
		char school[64];
		char hobby[64];
		memset(name,'\0', sizeof(name));
		memset(age,'\0', sizeof(age));
		memset(school,'\0', sizeof(school));
		memset(hobby,'\0', sizeof(hobby));

		int i=0;
		while(post_data[i]!='\0')
		{
			if(post_data[i]=='=' || post_data[i]=='&')
			{
				post_data[i]=' ';
			}
			i++;
		}

		//sscanf(post_data, "name=%s,&age=%s, &school=%s, &hobby=%s",name, age, school, hobby);
		sscanf(post_data,"%*s %s %*s %s %*s %s %*s %s",name,age,school,hobby);

		//std::string insert_data = "'qq', 12, 'wxyz', 'read'";
		//redis对应要插入两条记录
		string insert_data = "hmset student_five_class ";
		insert_data+=name;
		insert_data+=" ";
		insert_data+=age;
		insert_data+=" ";
		insert_data+=school;
		insert_data+=" ";
		insert_data+=hobby;
		insert_data+=" ";
		
		std::cout<<"<head>get success!"<<insert_data<<endl<<"</head>"<<std::endl;

    	const std::string _host = _remote_ip;
    	const std::string _user = _remote_user;
    	const std::string _passwd = _remote_passwd;
    	const std::string _db   = _remote_db;
    	const std::string _port   = _remote_port;
    	//sql_connecter conn(_host, _user, _passwd, _db);
    	//conn.begin_connect();
		//conn.insert_sql(insert_data);
		redisContext* redisConn = redisConnect(_host.c_str(),6379);
		if(redisConn!=NULL && redisConn->err)
			//cout<<"<head>connectiont error: "<<redisConn->errstr<<"</head>"<<endl;
			cout<<"connectiont error: "<<redisConn->errstr<<endl;
	
		redisReply* reply=(redisReply*)redisCommand(redisConn,"auth ireanlau");
		reply=(redisReply*)redisCommand(redisConn,insert_data.c_str());


		freeReplyObject(reply);
		redisFree(redisConn);

	std::cout<<"</body>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
}
