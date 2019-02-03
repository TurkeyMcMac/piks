exe = piks

all: $(exe)

$(exe): src
	$(CC) -O3 -o $@ src/*.c -lncurses

clean:
	$(RM) $(exe)
