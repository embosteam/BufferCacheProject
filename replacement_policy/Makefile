CC=gcc
CFLAGS=-g -Wall
LDLIBS=-pthread -fPIC
UNAME := $(shell uname)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
MAIN_FILE=*.c
MAIN_NAME=replacement_policy.o
SRCS_SHARED=$(wildcard ../shared/*.c)
OBJS_SHARED=$(SRCS_SHARED:.c=.o)
OBJS_SHARED_MOVED = $(OBJS_SHARED:../shared=.)

default: $(MAIN_NAME)
%.o : %.c
	@$(CC) -c $< $(LDLIBS)
$(MAIN_NAME): $(OBJS_SHARED) $(OBJS)
ifeq ($(UNAME),Darwin)
	@$(CC) -shared -o $@ $(OBJS) $(OBJS_SHARED_MOVED) $(LDLIBS)
else
	@$(CC) -c -o $@ $(OBJS) $(OBJS_SHARED_MOVED) $(LDLIBS)
endif
	@mv $@ ../$@

clean:
	rm -rf *.o