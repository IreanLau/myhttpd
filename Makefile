PWD=$(shell pwd)
CGI_PATH=$(PWD)/cgi_bin
SER_BIN=my_httpd
CLI_BIN=demo_client
SER_SRC=httpd.c
CLI_SRC=demo_client.c
INCLUDE=.
CC=gcc
FLAGS=-o #-D_DEBUG_
LDFLAGS=-lpthread -g#-static
LIB=

.PHONY:all
all:$(SER_BIN) $(CLI_BIN) cgi

$(SER_BIN):$(SER_SRC)
	$(CC) $(FLAGS)  $@ $^ $(LDFLAGS)
$(CLI_BIN):$(CLI_SRC)
	$(CC) $(FLAGS)  $@ $^ $(LDFLAGS)

.PHONY:cgi
cgi:
	for name in `echo $(CGI_PATH)`;\
		do\
			cd $$name;\
			make;\
			cd -;\
		done


.PHONY:output
output:all
	mkdir -p output/htdocs/cgi_bin
	cp my_httpd output
	cp demo_client output
	cp -rf conf output
	cp -rf log output
	cp -rf htdocs/*  output/htdocs
	for name in `echo $(CGI_PATH)`;\
		do\
			cd $$name;\
			make output;\
			cd -;\
		done

.PHONY:clean
clean:
	rm -rf $(SER_BIN) $(CLI_BIN) output 
	for name in `echo $(CGI_PATH)`;\
		do\
			cd $$name;\
			make clean;\
			cd -;\
		done
	

