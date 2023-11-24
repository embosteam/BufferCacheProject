CC=gcc
CFLAGS=-g -Wall
LDLIBS=-pthread
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
MAIN_FILE=*.c
MAIN_NAME=shared.o

default: $(MAIN_NAME)

$(MAIN_NAME): $(OBJS)
	@$(CC) -shared -o $@ $(OBJS) $(LDLIBS)
	@mv $@ ../$@
clean:
	rm -rf *.o