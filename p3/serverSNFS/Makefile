CC = gcc
CFLAGS = -c -g -pedantic -Wall
XMLRPC_C_CONFIG = xmlrpc-c-config
FUSE_C_CONFIG = pkg-config fuse

CFLAGS += $(shell $(XMLRPC_C_CONIFG) abyss-server --cflags)
LDFLAGS += $(shell $(XMLRPC_C_CONFIG) abyss-server --libs)

CFLAGS += $(shell (FUSE_C_CONFIG) --cflags)
LDFLAGS += $(shell (FUSE_C_CONFIG) --libs)

SERVER_EXEC = serverSNFS

SRCS = server.c
OBJS = ${SRCS:.c=.o}

all: ${SERVER_EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${SERVER_EXEC}: server.o
	${CC} -o ${SERVER_EXEC} ${OBJS} ${LDFLAGS}

test: ${SERVER_EXEC} kill
	# a very opinionated test for the server
	./${SERVER_EXEC} -mount ./temp -port 8080

kill: ${SERVER_EXEC}
	$(shell pkill serverSNFS)

clean:
	rm -rf ${SERVER_EXEC}
	rm -rf ${OBJS}
