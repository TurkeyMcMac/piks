exe = piks
man = piks.6
bin-dir = /usr/local/bin
man-dir = /usr/local/share/man/man6

c-flags = -std=c99 -Wall -Wextra -D_POSIX_C_SOURCE=200112 \
	-DPIKS_VERSION="\"`cat version`\"" $(CFLAGS)

all: $(exe) $(man)

$(exe): src version
	$(CC) $(c-flags) -o $@ src/*.c -lncurses

$(man): piks.6.in version
	sed "s/@@VERSION@@/`cat version`/g" piks.6.in > $@

install: $(exe) $(man)
	cp $(exe) $(bin-dir)/
	cp $(man) $(man-dir)/
	gzip $(man-dir)/$(man)

clean:
	$(RM) $(exe) $(man)
