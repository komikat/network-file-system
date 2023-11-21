client.o: client/main.c helper.c defs.h job_queue.c helper.h
	gcc client/main.c helper.c job_queue.c -o client.o

naming_server.o: naming_server/main.c helper.c defs.h job_queue.c helper.h
	gcc naming_server/main.c helper.c job_queue.c -o naming_server.o

storage_server.o: storage_server/main.c helper.c defs.h job_queue.c helper.h
	gcc storage_server/main.c storage_server/helper.c helper.c job_queue.c -o storage_server.o

clean:
	rm client.o naming_server.o storage_server.o
