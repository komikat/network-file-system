client.o: client/main.c
	gcc client/main.c helper.c -o client.o

naming_server.o: naming_server/main.c
	gcc naming_server/main.c helper.c -o naming_server.o
