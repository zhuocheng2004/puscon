
OBJS	:= start.o main.o

.PHONY: all
all: kernel

kernel: link.ld ${OBJS}
	${LD} -static -pie --no-dynamic-linker -o $@ -T link.ld ${OBJS}

%.o: %.s
	${CC} ${CPPFLAGS} -fPIC -o $@ -c $^

%.o: %.c
	${CC} ${CPPFLAGS} -fPIC -o $@ -c $^

.PHONY: clean
clean:
	@rm -rf kernel ${OBJS}
