CC = cc -g -Wall -Wextra -Werror

OBJ = moncat.o

%.o: %.c
	 $(CC) -c -o $@ $<

moncat: $(OBJ)
	gcc -o $@ $^

clean:
	rm -rf *.o moncat
