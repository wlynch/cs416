CC = gcc
CFLAGS = -c -g -pedantic -Wall
CFLAGS +=  $(shell pkg-config fuse --cflags)
LDFLAGS += $(shell pkg-config fuse --libs)
LDFLAGS += $(shell pkg-config libprotobuf-c --libs)

CLIENT_EXEC = clientSNFS

SRCS= client.c filesystem.c ../protobuf-model/ping.pb-c.c
OBJS = ${SRCS:.c=.o}

all: ${CLIENT_EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${CLIENT_EXEC}:${OBJS}
	${CC} -o ${CLIENT_EXEC} ${OBJS} ${LDFLAGS}

test: ${CLIENT_EXEC} kill
	# a very opinionated test scenario
	./${CLIENT_EXEC} -mount ./temp --tcp=localhost:8080

kill: ${CLIENT_EXEC}
	$(shell pkill clientSNFS)

clean:
	rm -rf ${CLIENT_EXEC}
	rm -rf ${OBJS}
