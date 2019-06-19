exe = piks

all: $(exe)

$(exe): src
	$(CC) -std=c99 -O3 -Wall -Wextra -flto $(CFLAGS) -o $@ src/*.c -lncurses

clean:
	$(RM) $(exe)
