CC=gcc
CFLAGS=-g -Wall
PWD := $(shell pwd)
LDLIBS=-pthread -L$(PWD:/delayed_write=/shared/)
UNAME := $(shell uname)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
SRCS_SHARED=$(wildcard ../shared/*.c)
OBJS_SHARED=$(SRCS_SHARED:.c=.o)
OBJS_SHARED_MOVED = $(OBJS_SHARED:../shared=.)
MAIN_FILE=*.c
MAIN_NAME=delayed_write.o

default: $(MAIN_NAME)

%.o : %.c
	@$(CC) -shared -c $< $(LDLIBS)
$(MAIN_NAME): $(OBJS_SHARED) $(OBJS)
ifeq ($(UNAME),Darwin)
	$(CC) -shared -o $@ $(OBJS) $(OBJS_SHARED_MOVED) $(LDLIBS)
else
	@$(CC) -static -o $@ $(OBJS) $(OBJS_SHARED_MOVED) $(LDLIBS)
endif
	@mv $@ ../$@
clean:
	rm -rf *.o