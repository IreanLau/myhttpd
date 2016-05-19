#ifndef _HTTPD_H_
#define _HTTPD_H_

#define HTTP_VERSION "HTTP/1.1"

#define BACK_LOG 5
#define COM_SIZE 1024
#define MAIN_PAGE "index.html"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include<sys/select.h>


#endif
