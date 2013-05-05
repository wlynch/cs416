CC = gcc

export PKG_CONFIG_PATH:=/filer/tmp2/contrib/lib/pkgconfig:${PKG_CONFIG_PATH}

CFLAGS = -c -g -Wall
CFLAGS += $(shell pkg-config fuse --cflags)
CFLAGS += $(shell PKG_CONFIG_PATH=${PKG_CONFIG_PATH} pkg-config libprotobuf-c --cflags)
LDFLAGS += $(shell pkg-config fuse --libs)
LDFLAGS += $(shell PKG_CONFIG_PATH=${PKG_CONFIG_PATH} pkg-config libprotobuf-c --libs)

SERVER_EXEC = serverSNFS

SRCS = filesystem.c server.c threading.c thread_helpers.c ../protobuf-model/ping.pb-c.c ../protobuf-model/fs.pb-c.c
OBJS = ${SRCS:.c=.o}

all: ${SERVER_EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${SERVER_EXEC}: ${OBJS}
	${CC} -o ${SERVER_EXEC} ${OBJS} ${LDFLAGS}

test: ${SERVER_EXEC} kill
	# a very opinionated test for the server
	./${SERVER_EXEC} -port 8080 -mount ./temp/

kill: ${SERVER_EXEC}
	$(shell pkill serverSNFS)

clean:
	rm -rf ${SERVER_EXEC}
	rm -rf ${OBJS}
