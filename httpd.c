#include"httpd.h"
#include<sys/select.h>


static void bad_request(int client)
{
	char buf[1024];
	sprintf(buf,"HTTP/1.0 400 BAD REQUSET\r\n");
	send(client,buf,strlen(buf),0);

	sprintf(buf,"content-type: text/html\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(client,buf,strlen(buf),0);
	sprintf(buf,"<p>your enter message is a bad request</p>");
	send(client,buf,strlen(buf),0);
}

static void not_found(int client)
{
}

void usage(const char *proc)
{
	printf("Usage : %s [IP] [PORT]\n",proc);
}

void print_debug(const char* msg)
{
	printf("%s\n",msg);
}

void echo_error_client(int client,int error_code)
{
	switch(error_code)
	{
		case 400://request error
			bad_request(client);
			break;
		case 404:
			not_found(client);
			break;
		case 500://server_ERROR
		//	server_error(client);
			break;
		case 503://server unavailable
		//server_unavailable(client);
			break;
		default:
			break;
	}
}
void clean_header(int client)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	int ret = 0;
	do
	{
		ret = get_line(client,buf,sizeof(buf));	
	}while(ret > 0 && strcmp(buf,"\n")!=0);
}

void print_log(const char*fun,int line ,int err_no,const char* err_str)
{
	printf("[%s : %d] [%d] [%s]\n",fun,line,err_no,err_str);
}


//
int get_line(int sock,char* buf,int max_len)
{
//	printf("AAAAAAA\n");
	if(!buf || max_len < 0)
	{
		return -1;
	}
	int i = 0;
	int n=0;
	char c = '\0';

	while( i<max_len-1 && c!='\n')
	{
		n = recv(sock,&c,1,0);
		if(n > 0)
		{//succ
			if(c == '\r')
			{
 	 	 	 	n= recv(sock,&c,1,MSG_PEEK);//嗅探
				if(n > 0 && c == '\n')
				{//windows
					recv(sock,&c,1,0);//delete
				}
				else
				{
					c ='\n';
 	 			}
			}
		buf[i++]=c;
	//	printf("%c ",c);
	//	fflush(stdout);
		}
		else
		{//failed
			c = '\n';
		}
	}
	printf("\n""");
	buf[i]='\0';
	return i;
}

void exe_cgi(int sock_client,const char*path,const char* method,const char* querry_string)
{
	char buf[COM_SIZE];
	int numchars = 0;
	int content_length = -1;
	int cgi_input[2]={0,0};
	int cgi_output[2]={0,0};

	pid_t id;

	if(strcasecmp(method,"GET") == 0)//GET
	{
		clean_header(sock_client);
	}
	else//POST
	{
		printf("++++++++++\n");
		do{
			memset(buf,'\0',sizeof(buf));
			numchars = get_line(sock_client,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length:",strlen("Content-Length:")) == 0)
			{
				content_length =atoi(&buf[16]);
		 		printf("coutent_length%d\n",content_length);
			}
		}while(numchars > 0 && strcmp(buf,"\n") != 0 );
		if(content_length == -1)
		{
	//		echo_error_client();
			return;
		}
	}

	//返回响应行
	memset(buf,'\0',sizeof(buf));
	strcpy(buf,HTTP_VERSION);
	strcat(buf," 200 OK\r\n\r\n");
	send(sock_client,buf,strlen(buf),0);

	if(pipe(cgi_input) == -1 )//error
	{
	//	echo_error_client();
		return;
	}
	if( pipe(cgi_output)== -1 )
	{
		close(cgi_input[0]);
		close(cgi_input[1]);
	//	echo_error_client();
		return;
	}

	if( (id=fork()) < 0 )
	{
		close(cgi_input[0]);
		close(cgi_input[1]);
		close(cgi_output[0]);
		close(cgi_output[1]);
	//	echo_error_client();
		return;
	}
	else if(id == 0) //child
	{
		char env_method[COM_SIZE];
		char env_query[COM_SIZE/10];
		char content_leng_env[COM_SIZE];

		memset(env_method,'\0',sizeof(env_method));
		memset(env_query,'\0',sizeof(env_query));
		memset(content_leng_env,'\0',sizeof(content_leng_env));
		
		close(cgi_input[1]);
		close(cgi_output[0]);

		dup2(cgi_input[0],0);
		dup2(cgi_output[1],1);

		sprintf(env_method,"REQUEST_METHOD=%s",method);

		putenv(env_method);

		if(strcasecmp("GET",method)==0 )
		{
			sprintf(env_query,"QUERY_STRING=%s",querry_string);
			putenv(env_query);
		}
		else//post
		{
			sprintf(content_leng_env,"CONTENT_LENGTH=%d",content_length);
			putenv(content_leng_env);
		}

		execl(path,path,NULL);
		exit(1);
	}
	else //father
	{
		close(cgi_input[0]);
		close(cgi_output[1]);

		int i =0;
		char c='\0';
		if(strcasecmp("POST",method) == 0)
		{
			for(;i<content_length;++i)
			{
 				recv(sock_client,&c,1,0);
	 			write(cgi_input[1],&c,1);
			}
		}
		while( read(cgi_output[0],&c,1)>0 )
		{
			send(sock_client,&c,1,0); //0 阻塞
		}
		close(cgi_input[1]);
		close(cgi_output[0]);

		waitpid(id,NULL,0);
	}

}

void echo_html(int client,const char*path,unsigned int file_size)
{
	//printf("-----------: %s\n", path);
	//printf("%d\n", file_size);
	if(path == NULL)
		return;

	int in_fd = open(path,O_RDONLY);
	if(in_fd < 0)
	{
	//	echo_error_client();
		return;
	}

	print_debug("open index.html success");
	char echo_line[1024];
	memset(echo_line,'\0',sizeof(echo_line));
	strncpy(echo_line,HTTP_VERSION,strlen(HTTP_VERSION)+1);
	strcat(echo_line," 200 OK");
	strcat(echo_line,"\r\n\r\n");
	send(client,echo_line,strlen(echo_line),0);
	print_debug("send echo head success");
	//send(client,"\r\n",strlen("\r\n"),0);
	//printf("%s\n", echo_line);
	if(sendfile(client,in_fd,NULL,file_size) < 0 )
	{
		close(in_fd);
		return;
	}
	print_debug("sendfile success");
	close(in_fd);
}

void *accept_request(void *arg)
{
	printf("......................\n");
	print_debug("get new line requset\n");
	pthread_detach(pthread_self());

	int cgi = 0;
	char* querry_string=NULL;//参数
	int sock_client = (int)arg;
	//iprintf("debug : sock_client : %d\n",sock_client);
	char method[COM_SIZE/10];
	char url[COM_SIZE];
	char buff[COM_SIZE];
	char path[COM_SIZE];

	memset(buff,'\0',sizeof(buff));
	memset(url,'\0',sizeof(url));
	memset(method,'\0',sizeof(method));
	memset(path,'\0',sizeof(path));

	if(get_line(sock_client,buff,sizeof(buff)) < 0 )
	{
		return (void*)-1;
	}
	print_debug(buff);
	print_debug(path);

	int i=0;
	int j=0;  //buff line index
	while(!isspace(buff[j]) && i < sizeof(method)-1 && j < sizeof(buff) )
	{
		method[i] = buff[j];
		i++,j++;
	}

	if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))
		return NULL;
	//clean space
	while( isspace(buff[j]) && j < sizeof(buff) )
	{
		j++;
	}
	//get url
	i=0;
	while(!isspace(buff[j]) && i < sizeof(url)-1 && j < sizeof(buff) )
	{
		url[i]=buff[j];
		i++,j++;
	}

	print_debug(method);
	print_debug(url);
	printf("method: %s\n",method);
	printf("url: %s\n",url);
	
	if(strcasecmp(method,"POST") == 0)
	{
		cgi = 1;
	}

	if( strcasecmp(method,"GET") == 0 )
	{
		querry_string=url;
		while(*querry_string != '?' && *querry_string != '\0')
		{
			querry_string++;
		}
		if(*querry_string == '?')//url = /XXX/XXX + arg
		{
			*querry_string='\0';
			querry_string++;
			cgi = 1;
		}
	}
	
	sprintf(path,"htdocs%s",url);

	if(path[strlen(path)-1] == '/')
	{
		strcat(path,MAIN_PAGE);
	}	

	print_debug(path);

	struct stat st;
	if(stat(path,&st) < 0)//failed ,not exist
	{
		//printf("_____________path%s\n",path);
		print_debug("miss cgi");
		clean_header(sock_client);
	//	echo_error_client();
	}
	else//file exist
	{
		if(S_ISDIR(st.st_mode))
		{
			strcat(path,"/");
			strcat(path,MAIN_PAGE);
		}
		else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode &  S_IXOTH)
		{
			cgi = 1;
		}
		else
		{

		}

		if(cgi)
		{
			printf("begin in cgi\n");
			exe_cgi(sock_client,path,method,querry_string);
		}
		else
		{
			clean_header(sock_client);
			print_debug("begin enter our echo html\n");
			echo_html(sock_client,path,st.st_size);
		}
	}
	close(sock_client);

	return NULL;
}

//succ ret sock,else exit proc
int start(short port)
{
	int listen_sock=socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock == -1)
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(1);
	}
	//端口复用
	int flag = 1;
	setsockopt(listen_sock,SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag));
	
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);//host->net
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	socklen_t len = sizeof(local);
	if( bind(listen_sock,(struct sockaddr*)&local,len) == -1 )
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(2);
	}

	if( listen(listen_sock,BACK_LOG) == -1 )
	{
		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
		exit(3);
	}

	return listen_sock;
}

//  ./httpd ip[option] port
int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}

	int port = atoi(argv[2]);
	int sock = start(port);

	struct sockaddr_in client;
	socklen_t len =0;



	fd_set accept_set;

	while(1)
	{
		int accept_sock = sock;
	//select
#ifdef _SELECT_
		FD_ZERO(&accept_set);
		FD_SET(accept_sock,&accept_set);
		struct timeval timeout={5,0};

		int select_res = select(accept_sock+1,&accept_set,NULL,NULL,&timeout);
	
		if(select_res == -1)
			perror("select");
	
		else if(select_res == 0)
			printf("No Data within five seconds.\n");
#endif
		else 
		{
	    	int new_sock = accept(sock,(struct sockaddr*)&client,&len);
	    	if(new_sock == -1)//failed
	    	{
	    		print_log(__FUNCTION__,__LINE__,errno,strerror(errno));
	    		continue; 
	    	}

	    	pthread_t new_thread;
	    	pthread_create(&new_thread,NULL,accept_request,(void*)new_sock);

		}
	}
	return 0;
}
