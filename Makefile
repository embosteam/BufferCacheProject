CC=gcc
CFLAGS=-g -Wall
LDLIBS=-pthread
OBJS=main.o delayed_write.o buffered_read.o replacement_policy.o shared.o
MAIN_FILE=*.c
MAIN_NAME=buffer_cache

DELAYED_WRITE_FOLDER=delayed_write
BUFFERED_READ_FOLDER=buffered_read
REPLACEMENT_POLICY_FOLDER=replacement_policy
SHARED_FOLDER=shared

DELAYED_WRITE_FILE=$(DELAYED_WRITE_FOLDER)/*.c $(DELAYED_WRITE_FOLDER)/*.h
BUFFERED_READ_FILE=$(BUFFERED_READ_FOLDER)/*.c $(BUFFERED_READ_FOLDER)/*.h
REPLACEMENT_POLICY_FILE=$(REPLACEMENT_POLICY_FOLDER)/*.c $(REPLACEMENT_POLICY_FOLDER)/*.h
SHARED_FILE=$(SHARED_FOLDER)/*.c $(SHARED_FOLDER)/*.h

default: $(MAIN_NAME)

$(MAIN_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDLIBS)
%.o : %.c
	$(CC) -c $< $(LDLIBS)
delayed_write.o: $(DELAYED_WRITE_FILE)
	$(CC) -o $@ -c $< $(LDLIBS)
buffered_read.o: $(BUFFERED_READ_FILE)
	$(CC) -o $@ -c $< $(LDLIBS)
replacement_policy.o: $(REPLACEMENT_POLICY_FILE)
	$(CC) -o $@ -c $< $(LDLIBS)
shared.o: $(SHARED_FILE)
	$(CC) -o $@ -c $< $(LDLIBS)
main.o: $(MAIN_FILE)
	$(CC) -o $@ -c $< $(LDLIBS)
clean:
	rm -rf *.o
clean-all:clean
	rm $(MAIN_NAME)

	