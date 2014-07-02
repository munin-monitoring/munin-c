PLUGINS:=ok_plugin nb_env

all: $(PLUGINS)

%.h:
%.c:

# Generic plugin compilation
%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

# Common plugin code
common.o: common.c common.h
	$(CC) -c -o $@ $< $(CFLAGS)

# Plugin executable gen
%: %.o common.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(PLUGINS)

.PHONY: all clean
.SUFFIXES:
MAKEFLAGS += -r
