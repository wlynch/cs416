CC = gcc
CFLAGS = -c -g -pedantic -Wall
XMLRPC_C_CONFIG = xmlrpc-c-config
CFLAGS += $(shell $(XMLRPC_C_CONIFG) client --cflags)
CFLAGS +=  $(shell pkg-config fuse --cflags)
LDFLAGS += $(shell $(XMLRPC_C_CONFIG) client --libs)
LDFLAGS += $(shell pkg-config fuse --libs)

CLIENT_EXEC = clientSNFS

SRCS= client.c filesystem.c
OBJS = ${SRCS:.c=.o}

all: ${CLIENT_EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${CLIENT_EXEC}:${OBJS}
	${CC} -o ${CLIENT_EXEC} ${OBJS} ${LDFLAGS}

test: ${CLIENT_EXEC} kill
	# a very opinionated test scenario
	./${CLIENT_EXEC} -mount ./temp -port 8080 -address localhost

kill: ${CLIENT_EXEC}
	$(shell pkill clientSNFS)

clean:
	rm -rf ${CLIENT_EXEC}
	rm -rf ${OBJS}
