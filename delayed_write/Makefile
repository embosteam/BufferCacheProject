CC=gcc
CFLAGS=-g -Wall -pedantic -lms
PWD := $(shell pwd)
#LDLIBS=-pthread -fPIC -L$(PWD:/delayed_write=/shared/)
LDLIBS=-pthread -fPIC -lpthread -fno-stack-protector
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
	@$(CC) -shared -c $< $(LDLIBS) $(CFLAGS)
$(MAIN_NAME): $(OBJS_SHARED) $(OBJS)
ifeq ($(UNAME),Darwin)
	$(CC) -shared -o $@ $(OBJS) $(OBJS_SHARED_MOVED) $(LDLIBS)
else
	@$(CC) -c -o $@ $(OBJS) $(OBJS_SHARED_MOVED) $(LDLIBS)
endif
	@mv $@ ../$@
clean:
	rm -rf *.o