.PHONY: client server compile

compile: client_work forking_server

client: client_work
	./client_work

client_work: basic_client.o pipe_networking.o
	gcc -o client_work basic_client.o pipe_networking.o

server: forking_server
	./forking_server

forking_server: forking_server.o pipe_networking.o
	gcc -o forking_server forking_server.o pipe_networking.o

basic_client.o: basic_client.c pipe_networking.h
	gcc -c basic_client.c

forking_server.o: forking_server.c pipe_networking.h
	gcc -c forking_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

clean:
	rm -f *.o basic_client client persistant_server server
	rm -f *~
