#!/bin/bash

MYSQL_DB="remote_table"
FLAG="-N -r"
SQLSHELL="events_to_redis.sql"
REDISAUTH="ireanlau"
mysql -uroot -p ${MYSQL_DB} ${FLAG} <  ${SQLSHELL} | redis-cli --pipe -a ${REDISAUTH}
