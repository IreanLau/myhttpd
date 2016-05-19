#!/bin/bash
redisServer="/usr/local/redis/bin/redis-server"
conf=" /usr/local/redis/etc/redis.conf"
ROOT_PATH=`pwd`
${redisServer} ${conf}

${ROOT_PATH}/redisc/redisToMysql.sh
make clean;make;make output
${ROOT_PATH}/redisc/mysqlToRedis.sh
