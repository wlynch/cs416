CC = gcc
CCFLAGS = -c -g -pedantic -Wall
SERVER_LIBS = -L/usr/local/lib   -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss  -lpthread -lxmlrpc -lxmlrpc_util  libxmlrpc_xmlparse.a libxmlrpc_xmltok.a


SERVER_EXEC = serverSNFS
OBJS = server.o


all: ${SERVER_EXEC}

${SERVER_EXEC}: server.o
	${CC} -o ${SERVER_EXEC} server.o ${SERVER_LIBS}

server.o: server.c
	${CC} ${CCFLAGS} server.c

clean: 
	rm -rf ${SERVER_EXEC}
	rm -rf *.o
