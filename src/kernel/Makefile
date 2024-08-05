
OBJS	:= start_${ARCH}.o main.o

.PHONY: all
all: kernel

kernel: link.ld ${OBJS}
	${CC} -pie -nostdlib -o $@ ${OBJS}

%.o: %.s
	${CC} ${CLAGS} -fPIC -o $@ -c $^

%.o: %.c
	${CC} ${CFLAGS} -fPIC -o $@ -c $^

.PHONY: clean
clean:
	@rm -rf kernel *.o
