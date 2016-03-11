FIND_PATH=-I/home/lau/project/myhttpd/sql_connect -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
SRC_PATH=/home/lau/project/myhttpd/sql_connect/sql_connect.cpp
ADD_FLAGS=-ldl -lrt -pthread
.PHONY:all
all:debug show_student_cgi register_cgi



debug:debug.c
	gcc -o debug debug.c
show_student_cgi:show_student_cgi.cpp $(SRC_PATH)
	g++ -o $@ $^ $(FIND_PATH) $(ADD_FLAGS) -g
register_cgi:register_cgi.cpp $(SRC_PATH)
	g++ -o $@ $^ $(FIND_PATH) $(ADD_FLAGS)
.PHONY:output
output:
	cp debug ../output/htdocs/cgi_bin
	cp show_student_cgi ../output/htdocs/cgi_bin
	cp register_cgi ../output/htdocs/cgi_bin
.PHONY:clean
clean:
	rm -rf debug show_student_cgi register_cgi
