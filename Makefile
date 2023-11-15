client.o: client/main.c
	gcc client/main.c helper.c -o client.o

naming_server.o: naming_server/main.c
	gcc naming_server/main.c helper.c -o naming_server.o

storage_server.o: storage_server/main.c
	gcc storage_server/main.c storage_server/helper.c helper.c -o storage_server.o