exe = piks

c-flags = -std=c99 -O3 -Wall -Wextra -flto $(CFLAGS)

all: $(exe)

$(exe): src
	$(CC) $(c-flags) -o $@ src/*.c -lncurses

clean:
	$(RM) $(exe)
