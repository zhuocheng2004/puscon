
.PHONY: all
all: libpuscon.a

libpuscon.a: puscon.o syscall.o printk.o util/libpuscon-util.o fs/libpuscon-fs.o
	ar rcs $@ $^

util/libpuscon-util.o: FORCE
	${MAKE} -C util/

fs/libpuscon-fs.o: FORCE
	${MAKE} -C fs/

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	${MAKE} -C fs/ clean
	${MAKE} -C util/ clean
	@rm -rf *.a *.o

.PHONY: FORCE
FORCE:
