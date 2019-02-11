exe = piks

all: $(exe)

$(exe): src
	$(CC) -O3 -Wall -Wextra -o $@ src/*.c -lncurses

clean:
	$(RM) $(exe)
