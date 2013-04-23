CC = gcc
CFLAGS = -c -g -pedantic -Wall
CFLAGS += $(shell pkg-config fuse --cflags)
LDFLAGS += $(shell pkg-config fuse --libs)

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
