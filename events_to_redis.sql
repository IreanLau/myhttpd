-- events_to_redis.sql
 
 SELECT CONCAT(
  "*6\r\n",
  '$', LENGTH(redis_cmd), '\r\n',
  redis_cmd, '\r\n',
  '$', LENGTH(redis_key), '\r\n',
  redis_key, '\r\n',
  '$', LENGTH(hkey), '\r\n',
  hkey, '\r\n',
  '$', LENGTH(hval), '\r\n',
  hval, '\r\n',
  '$', LENGTH(hkey2), '\r\n',
  hkey2, '\r\n',
  '$', LENGTH(hval2), '\r\n',
  hval2, '\r'
)
FROM (
  SELECT
    'HMSET' as redis_cmd,
	  'student_five_class' AS redis_key,
	  name AS hkey,
	  age AS hval,
	  school AS hkey2,
	  hobby AS hval2
	  FROM student_five_class
	) AS t
