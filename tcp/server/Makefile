.PHONY: clean
server: server.o
	gcc -Wall server.o command_handlers.h common.h data_structure.h global.h util.h -o server -lpthread
server.o:server.c
	gcc -c server.c

clean:
	rm *.o server
