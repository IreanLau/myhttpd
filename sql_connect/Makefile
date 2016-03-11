BIN=sql_connect
CC=g++
INCLUDE=-I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient 
SRC=sql_connect.cpp


$(BIN):$(SRC)
	$(CC) -o $@ $^ $(INCLUDE) -lpthread -ldl -lrt -g -D_DEBUG_
.PHONY:clean
clean:
	rm -rf $(BIN)
