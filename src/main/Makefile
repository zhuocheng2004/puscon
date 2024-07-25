
.PHONY: all
all: puscon

puscon: main.o ../libpuscon.a
	${CC} -o $@ $^

.PHONY: clean
clean:
	@rm -rf puscon *.o

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^
