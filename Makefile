exe = piks
bin-dir = /usr/local/bin

c-flags = -std=c99 -Wall -Wextra -D_POSIX_C_SOURCE=200112 \
	-DPIKS_VERSION="\"`cat version`\"" $(CFLAGS)

all: $(exe)

$(exe): src version
	$(CC) $(c-flags) -o $@ src/*.c -lncurses

install: $(exe)
	cp $(exe) $(bin-dir)/

clean:
	$(RM) $(exe)
