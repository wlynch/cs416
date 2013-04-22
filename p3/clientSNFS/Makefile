CC = gcc
CCFLAGS = -c -g -pedantic -Wall -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse
CLIENT_LIBS = -L/usr/local/lib -lxmlrpc_client -lpthread -lxmlrpc -lxmlrpc_util  ../lib/libxmlrpc_xmlparse.a ../lib/libxmlrpc_xmltok.a -L/lib64 -lfuse -lrt -ldl


CLIENT_EXEC = clientSNFS
OBJS = client.o filesystem.o 


all: ${CLIENT_EXEC}

${CLIENT_EXEC}:${OBJS} 
	${CC} -o ${CLIENT_EXEC} ${OBJS} ${CLIENT_LIBS}

filesystem.o: filesystem.c
	${CC} ${CCFLAGS} filesystem.c

client.o: client.c
	${CC} ${CCFLAGS} client.c

clean: 
	rm -rf ${CLIENT_EXEC}
	rm -rf *.o
