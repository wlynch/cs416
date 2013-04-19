CC = gcc
CCFLAGS = -c -g -pedantic -Wall
SERVER_LIBS = -L/usr/local/lib   ../lib/libxmlrpc_server_abyss.a ../lib/libxmlrpc_server.a ../lib/libxmlrpc_abyss.a  -lpthread ../lib/libxmlrpc.a ../lib/libxmlrpc_util.a  ../lib/libxmlrpc_xmlparse.a ../lib/libxmlrpc_xmltok.a


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
