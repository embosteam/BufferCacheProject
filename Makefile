DELAYED_WRITE_FOLDER=delayed_write
BUFFERED_READ_FOLDER=buffered_read
REPLACEMENT_POLICY_FOLDER=replacement_policy
SHARED_FOLDER=shared

CC=gcc
CFLAGS=-g -Wall -pedantic
LDLIBS=-pthread -fPIC -lpthread  -fno-stack-protector
#OBJS=shared.o main.o delayed_write.o buffered_read.o replacement_policy.o
OBJS = buffer.o
MAIN_FILE=buffer.c
MAIN_NAME=buffer_cache

DELAYED_WRITE_FILE=$(DELAYED_WRITE_FOLDER)/*.c $(DELAYED_WRITE_FOLDER)/*.h
BUFFERED_READ_FILE=$(BUFFERED_READ_FOLDER)/*.c $(BUFFERED_READ_FOLDER)/*.h
REPLACEMENT_POLICY_FILE=$(REPLACEMENT_POLICY_FOLDER)/*.c $(REPLACEMENT_POLICY_FOLDER)/*.h
SHARED_FILE=$(SHARED_FOLDER)/*.c $(SHARED_FOLDER)/*.h

SRCS_SHARED=$(wildcard shared/*.c)
SRCS_BUFFERED_READ=$(wildcard buffered_read/*.c)
SRCS_REPLACEMENT_POLICY=$(wildcard replacement_policy/*.c)
SRCS_DELAYED_WRITE=$(wildcard delayed_write/*.c)

OBJS_SHARED=$(SRCS_SHARED:.c=.o)
OBJS_BUFFERED_READ=$(SRCS_BUFFERED_READ:.c=.o)
OBJS_REPLACEMENT_POLICY=$(SRCS_REPLACEMENT_POLICY:.c=.o)
OBJS_DELAYED_WRITE=$(SRCS_DELAYED_WRITE:.c=.o)

OBJS_SHARED_MOVED=$(OBJS_SHARED: shared=.)
OBJS_BUFFERED_READ_MOVED=$(OBJS_BUFFERED_READ: buffered_read=.)
OBJS_REPLACEMENT_POLICY_MOVED=$(OBJS_REPLACEMENT_POLICY: replacement_policy=.)
OBJS_DELAYED_WRITE_MOVED=$(OBJS_DELAYED_WRITE: delayed_write=.)

default: $(MAIN_NAME)

$(MAIN_NAME):  $(OBJS_SHARED) $(OBJS_BUFFERED_READ) $(OBJS_REPLACEMENT_POLICY) $(OBJS_DELAYED_WRITE) $(OBJS) # $(OBJS)
#	$(CC) -o $@  $(OBJS) $(LDLIBS)
	$(CC) -o $@ $(OBJS) $(OBJS_SHARED) $(OBJS_BUFFERED_READ) $(OBJS_REPLACEMENT_POLICY) $(OBJS_DELAYED_WRITE) $(LDLIBS)
#%.o : %.c
#	@$(CC) -c $< $(LDLIBS)
delayed_write.o:# $(DELAYED_WRITE_FILE)
	@$(MAKE) -C $(DELAYED_WRITE_FOLDER)
#	@$(CC) -o $@ -c $< $(LDLIBS)
buffered_read.o:# $(BUFFERED_READ_FILE)
	@$(MAKE) -C $(BUFFERED_READ_FOLDER)
#	@$(CC) -o $@ -c $< $(LDLIBS)
replacement_policy.o:# $(REPLACEMENT_POLICY_FILE)
	@$(MAKE) -C $(REPLACEMENT_POLICY_FOLDER)
#	@$(CC) -o $@ -c $< $(LDLIBS)
shared.o:  # $(SHARED_FILE)
	@$(MAKE) -C $(SHARED_FOLDER)
#	$(CC) -o $@ -c $(wildcard $(SHARED_FOLDER)/*.o) $(LDLIBS)
main.o:$(MAIN_FILE)
	$(CC) -o $@ -c $< $(LDLIBS)
clean: 
	rm -rf *.o
	@$(MAKE) -C $(DELAYED_WRITE_FOLDER) clean
	@$(MAKE) -C $(BUFFERED_READ_FOLDER) clean
	@$(MAKE) -C $(REPLACEMENT_POLICY_FOLDER) clean
	@$(MAKE) -C $(SHARED_FOLDER) clean
clean-all:clean
	rm $(MAIN_NAME)

	