CC = gcc
CCFLAGS = -c -g -pedantic -Wall
CLIENT_LIBS = -L/usr/local/lib -lxmlrpc_client -lpthread -lxmlrpc -lxmlrpc_util  ../lib/libxmlrpc_xmlparse.a ../lib/libxmlrpc_xmltok.a


CLIENT_EXEC = clientSNFS
OBJS = CLIENT.o


all: ${CLIENT_EXEC}

${CLIENT_EXEC}: client.o
	${CC} -o ${CLIENT_EXEC} client.o ${CLIENT_LIBS}

client.o: client.c
	${CC} ${CCFLAGS} client.c

clean: 
	rm -rf ${CLIENT_EXEC}
	rm -rf *.o
