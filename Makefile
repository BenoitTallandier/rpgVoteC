code : 
	gcc -o code code.c -lcurl -lm $(mysql_config --cflags --libs) -L/usr/lib/mysql -lmysqlclient
 clean:
	rm code
