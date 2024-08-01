
%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $^
