exe = piks

all: $(exe)

$(exe): src
	$(CC) -O3 -Wall -Wextra $(CFLAGS) -o $@ src/*.c -lncurses

clean:
	$(RM) $(exe)
