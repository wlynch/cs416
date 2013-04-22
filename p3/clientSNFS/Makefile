CC = gcc
CCFLAGS = -c -g -pedantic -Wall -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse
XMLRPC_C_CONFIG = xmlrpc-c-config
CLIENT_FLAGS = $(shell $(XMLRPC_C_CONIFG) client --cflags)
CLIENT_LIBS = $(shell $(XMLRPC_C_CONFIG) client --libs) 


CLIENT_EXEC = clientSNFS
OBJS = client.o filesystem.o 


all: ${CLIENT_EXEC}

${CLIENT_EXEC}:${OBJS} 
	${CC} -o ${CLIENT_EXEC} ${OBJS} ${CLIENT_LIBS}

filesystem.o: filesystem.c
	${CC} ${CLIENT_FLAGS} ${CCFLAGS} filesystem.c

client.o: client.c
	${CC} ${CLIENT_FLGAS} ${CCFLAGS} client.c

clean: 
	rm -rf ${CLIENT_EXEC}
	rm -rf *.o
