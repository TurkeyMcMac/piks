exe = piks
bin-dir = /usr/local/bin

c-flags = -std=c99 -Wall -Wextra -D_POSIX_C_SOURCE=200112 $(CFLAGS)

all: $(exe)

$(exe): src
	$(CC) $(c-flags) -o $@ src/*.c -lncurses

install: $(exe)
	cp $(exe) $(bin-dir)/

clean:
	$(RM) $(exe)
