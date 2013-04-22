CC = gcc
CCFLAGS = -c -g -pedantic -Wall
XMLRPC_C_CONFIG = xmlrpc-c-config
FUSE_C_CONFIG = pkg-config fuse
SERVER_FLAGS = $(shell $(XMLRPC_C_CONIFG) abyss-server --cflags)
SERVER_LIBS = $(shell $(XMLRPC_C_CONFIG) abyss-server --libs)

SERVER_FLAGS += $(shell (FUSE_C_CONFIG) --cflags)
SERVER_LIBS += $(shell (FUSE_C_CONFIG) --libs)

SERVER_EXEC = serverSNFS
OBJS = server.o


all: ${SERVER_EXEC}

${SERVER_EXEC}: server.o
	${CC} -o ${SERVER_EXEC} server.o ${SERVER_LIBS}

server.o: server.c
	${CC} ${SERVER_FLAGS} ${CCFLAGS} server.c

clean: 
	rm -rf ${SERVER_EXEC}
	rm -rf *.o
