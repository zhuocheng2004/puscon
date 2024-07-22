
.PHONY: all
all: puscon

puscon: main.o ../libpuscon.a ../util/libpuscon-util.a
	${CC} -o $@ $^

.PHONY: clean
clean:
	rm -rf puscon *.o

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^
