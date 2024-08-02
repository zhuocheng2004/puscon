
OBJS	:= start_${ARCH}.o main.o

.PHONY: all
all: kernel

kernel: link.ld ${OBJS}
ifdef ANDROID
	${CC} -pie -o $@ -nostdlib ${OBJS}
else
#	${LD} -static -pie --no-dynamic-linker -o $@ -T link.ld ${OBJS}
	${CC} -pie -static -nostdlib -o $@ ${OBJS}
endif

%.o: %.s
	${CC} ${CLAGS} -fPIC -o $@ -c $^

%.o: %.c
	${CC} ${CFLAGS} -fPIC -o $@ -c $^

.PHONY: clean
clean:
	@rm -rf kernel *.o
