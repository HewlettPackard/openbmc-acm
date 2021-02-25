TARGET=acm-poll
OBJS = acm_poll.o acm_extra.o acm_pwm.o acm_service.o
CFILES = acm_extra.c acm_pwm.c acm_service.c acm_poll.c
INCLUDES =
LIBS = -llibsystemd -lpthread
PKG = `pkg-config --cflags --libs libsystemd` -lpthread
CC = gcc

all:${OBJS}
	${CC} ${CFILES} ${CFLAGS} ${INCLUDES} -o $(TARGET) ${PKG}

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $^ -o $@

.PHONY: clean
clean:
	-rm -f *.o $(TARGET)
	