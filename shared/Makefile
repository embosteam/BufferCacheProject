CC=gcc
CFLAGS=-g -Wall
LDLIBS=-pthread
UNAME := $(shell uname)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
MAIN_FILE=*.c
MAIN_NAME=shared.o

default: $(MAIN_NAME)
%.o : %.c
	@$(CC) -c $< $(LDLIBS)
$(MAIN_NAME): $(OBJS)
ifeq ($(UNAME),Darwin)
	@$(CC) -shared -o $@ $(OBJS) $(LDLIBS)
else
	@$(CC) -static -o $@ $(OBJS) $(LDLIBS)
endif
	@mv $@ ../$@
clean:
	rm -rf *.o